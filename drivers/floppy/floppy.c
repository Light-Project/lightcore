/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "floppy"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <bits.h>
#include <irq.h>
#include <initcall.h>
#include <spinlock.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/block.h>
#include <driver/floppy/floppy.h>
#include <printk.h>

#include <asm/delay.h>
#include <asm/proc.h>
#include <asm/io.h>

struct floppy_info {
    unsigned int size;      /* nr of sectors total  */
    unsigned int cylinder;  /* nr of cylinder       */
    unsigned int head;      /* head per cylinder    */
    unsigned int sector;    /* sectors per cylinder */

    unsigned char track;
    unsigned char rate;
};

struct fdd_device {
    struct fdc_device *host;
    struct floppy_info *type;
    struct block_device block;
    unsigned char port;
};

#define block_to_fdd(blk) \
    container_of(blk, struct fdd_device, block)

struct fdc_device {
    struct fdd_device fdd[DRIVER_PER_FDC];
    struct irqchip_channel *irqchip;
    struct device *dev;

    enum fdc_version version;
    resource_size_t base;
    bool irq_occur;

    uint8_t dor, dtr;
};

#define CMD_WAIT_IRQ    BIT(0)
#define CMD_DIRVER      BIT(1)
#define CMD_PIO         BIT(2)
#define CMD_DMA         BIT(3)

struct floppy_cmd {
    uint8_t type;               /* Transmit type    */
    unsigned char txlen;        /* Length to send   */
    unsigned char rxlen;        /* Length to reply  */
    unsigned char replen;       /* Actual reply     */
    unsigned char retry;        /* Retry count      */
    uint8_t txcmd[FD_MAX_CMD_SIZE];
    uint8_t rxcmd[FD_MAX_REPLY_SIZE];

    /* transmit ignored */
    struct fdd_device *fdd;     /* Which device     */
    bool write;                 /* Read or write    */
    char *buffer;               /* Buffer pointer   */
    size_t buffersize;          /* Buffer len       */
};

static SPIN_LOCK(floppy_lock);
static struct fdc_device *current_fdc;

// static struct floppy_info floppy_type[] = {{
//     },{ /* 1 - 360KB PC */
//         .size =  720, .cylinder = 40, .head = 2, .sector =  9,
//         .rate = FLOPPY_DSR_RATE_300K,
//     },{ /* 2 - 1.2MB AT */
//         .size = 2400, .cylinder = 80, .head = 2, .sector = 15,
//         .rate = FLOPPY_DSR_RATE_500K,
//     },{ /* 3 - 360KB SS */
//         .size =  720, .cylinder = 80, .head = 1, .sector =  9,
//         .rate = FLOPPY_DSR_RATE_250K,
//     },
// };

static __always_inline uint8_t
fdc_in(struct fdc_device *fdc, int reg)
{
    return inb(fdc->base + reg);
}

static __always_inline void
fdc_out(struct fdc_device *fdc, int reg, uint8_t value)
{
    outb(fdc->base + reg, value);
}

static __always_inline uint8_t
dor_mask(struct fdc_device *fdc, uint8_t off, uint8_t on)
{
    fdc->dor = (fdc->dor & ~off) | on;
    fdc_out(fdc, FLOPPY_DOR, fdc->dor);
    return fdc->dor;
}

static inline state fdc_wait_ready(struct fdc_device *fdc)
{
    unsigned int timeout = 10000;
    uint8_t val;

    while (--timeout) {
        val = fdc_in(fdc, FLOPPY_MSR);
        if (val & FLOPPY_MSR_RQM)
            break;
    }

    return timeout ? val : -EBUSY;
}

static irqreturn_t floppy_interrupt(irqnr_t intnr, void *fdc)
{
    if (!current_fdc)
        return IRQ_RET_NONE;
    current_fdc->irq_occur = true;
    return IRQ_RET_HANDLED;
}

static state floppy_wait_irq(struct fdc_device *fdc)
{
    unsigned int timeout = 50000;
    current_fdc = fdc;

    while (!fdc->irq_occur && --timeout)
        udelay(100);

    if (!timeout)
        return -EBUSY;

    fdc->irq_occur = false;
    return -ENOERR;
}

static inline state fdc_enable(struct fdc_device *fdc)
{
    state ret;

    dor_mask(fdc, FLOPPY_DOR_RESET, FLOPPY_DOR_IRQ);
    udelay(20);
    dor_mask(fdc, 0, FLOPPY_DOR_RESET | FLOPPY_DOR_IRQ);

    if ((ret = floppy_wait_irq(fdc))) {
        dev_err(fdc->dev, "fail to enable\n");
        return ret;
    }

    return -ENOERR;
}

static void fdc_reset(struct fdc_device *fdc)
{
    if (fdc->version >= FDC_82072A) {
        fdc_out(fdc, FLOPPY_DSR, (fdc->dtr &
                     FLOPPY_DSR_RATE_MASK) | FLOPPY_DSR_SWRST);
    } else {
        dor_mask(fdc, FLOPPY_DOR_RESET, 0);
        udelay(20);
        dor_mask(fdc, 0, FLOPPY_DOR_RESET);
    }
}

static void driver_select(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    dor_mask(fdc, FLOPPY_DOR_DSEL_MASK, fdd->port);
}

static void motor_power(struct fdd_device *fdd, bool enable)
{
    struct fdc_device *fdc = fdd->host;

    if (!!(fdc->dor & (FLOPPY_DOR_MOTOR_A << fdd->port)) == enable)
        return;

    if (enable) {
        dor_mask(fdc, 0, FLOPPY_DOR_MOTOR_A << fdd->port);
        mdelay(300); /* wait for stability */
    } else {
        dor_mask(fdc, FLOPPY_DOR_MOTOR_A << fdd->port, 0);
    }
}

static state floppy_transmit(struct fdc_device *fdc, struct floppy_cmd *cmd)
{
    unsigned int timeout, count;
    uint8_t val, residue = 5;
    state ret = -ENOERR;
    cmd->retry = -1;

retry:
    if (!residue--)
        return -EBUSY;

    cmd->retry++;

    if (cmd->type & CMD_DIRVER) {
        motor_power(cmd->fdd, true);
        driver_select(cmd->fdd);
    }

    if (cmd->type & CMD_DMA) {

    }

    /* sends command to the fdc */
    for (count = 0; cmd->txlen != count; ++count) {
        timeout = 50;

        while (--timeout) {
            val = fdc_in(fdc, FLOPPY_MSR) &
                (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO);
            if (val == FLOPPY_MSR_RQM)
                break;
            mdelay(1);
        } if (!timeout) {
            dev_debug(fdc->dev, "wait timeout while send data\n");
            fdc_reset(fdc);
            goto retry;
        }

        fdc_out(fdc, FLOPPY_FIFO, cmd->txcmd[count]);
    }

    if (cmd->type & CMD_PIO)
        if (!(fdc_in(fdc, FLOPPY_MSR) & FLOPPY_MSR_NDMA))
            goto out;

wait:
    if (cmd->type & CMD_WAIT_IRQ) {
        if ((ret = floppy_wait_irq(fdc))) {
            dev_warn(fdc->dev, "timeout while wait handle\n");
            fdc_reset(fdc);
            goto retry;
        }
    }

    if (cmd->type & CMD_PIO) {
        timeout = 50;

        while (--timeout) {
            val = fdc_in(fdc, FLOPPY_MSR) &
                       (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA);
            if (val == (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA))
                break;
            mdelay(1);
        } if (!timeout) {
            dev_warn(fdc->dev, "wait timeout while transfer data\n");
            fdc_reset(fdc);
            goto retry;
        }

        if (cmd->buffersize) {
            if (cmd->write)
                fdc_out(fdc, FLOPPY_FIFO, *cmd->buffer++);
            else
                *cmd->buffer++ = fdc_in(fdc, FLOPPY_FIFO);
            cmd->buffersize--;
        } else {
            if (cmd->write)
                fdc_out(fdc, FLOPPY_FIFO, 0x00);
            else
                fdc_in(fdc, FLOPPY_FIFO);
            ret = -ENOMEM;
        }

        goto wait;
    }

out:
    /* gets the response from the fdc */
    while (cmd->rxlen != cmd->replen) {
        timeout = 50;

        while (--timeout) {
            val = fdc_in(fdc, FLOPPY_MSR) & (FLOPPY_MSR_RQM |
                FLOPPY_MSR_DIO | FLOPPY_MSR_NDMA | FLOPPY_MSR_CB);
            if (val & FLOPPY_MSR_RQM)
                break;
            mdelay(1);
        } if (!timeout) {
            dev_warn(fdc->dev, "wait timeout while receive cmd\n");
            fdc_reset(fdc);
            goto retry;
        }

        if ((val & ~FLOPPY_MSR_CB) == FLOPPY_MSR_RQM)
            break;

        if (val == (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_CB))
            cmd->rxcmd[cmd->replen++] = fdc_in(fdc, FLOPPY_FIFO);
        else {
            fdc_reset(fdc);
            goto retry;
        }
    }

    val = fdc_in(fdc, FLOPPY_MSR) & (FLOPPY_MSR_RQM |
        FLOPPY_MSR_DIO | FLOPPY_MSR_NDMA | FLOPPY_MSR_CB);
    if (FLOPPY_MSR_RQM != val) {
        fdc_reset(fdc);
        goto retry;
    }

    return ret;
}

static enum fdc_version fdc_version(struct fdc_device *fdc)
{
    struct floppy_cmd cmd;
    state ret;

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_CMD_DUMPREG;
    ret = floppy_transmit(fdc, &cmd);

    if (ret || (cmd.replen <= 0))
        return FDC_NONE;

    if ((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        dev_info(fdc->dev, "version 8272A\n");
        return FDC_8272A;
    }

    if (cmd.replen != 10) {
        dev_info(fdc->dev, "DUMPREGS: return of %d bytes.\n", cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_CMD_PERPENDICULAR_MODE;
    ret = floppy_transmit(fdc, &cmd);

    if (!ret) {
        dev_info(fdc->dev, "version 82072A\n");
        return FDC_82072A;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_CMD_LOCK;
    ret = floppy_transmit(fdc, &cmd);

    if ((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        dev_info(fdc->dev, "version pre 82077\n");
        return FDC_82077_ORIG;
    }

    if ((cmd.replen != 1) && (cmd.rxcmd[0] != 0x00)) {
        dev_info(fdc->dev, "UNLOCK: return of %d bytes.\n", cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_CMD_PARTID;
    ret = floppy_transmit(fdc, &cmd);

    if (cmd.replen != 1) {
        dev_info(fdc->dev, "PARTID: return of %d bytes.\n", cmd.replen);
        return FDC_UNKNOWN;
    }

    if (cmd.rxcmd[0] == 0x80) {
        dev_info(fdc->dev, "version post 82077\n");
        return FDC_82077;
    }

    switch (cmd.rxcmd[0] >> 5) {
        case 0x00:
            dev_info(fdc->dev, "version 82078\n");
            return FDC_82078;
        case 0x01:
            dev_info(fdc->dev, "version 44pin 82078\n");
            return FDC_82078;
        case 0x02:
            dev_info(fdc->dev, "version S82078B\n");
            return FDC_S82078B;
        case 0x03:
            dev_info(fdc->dev, "version PC87306\n");
            return FDC_82078;
    }

    dev_info(fdc->dev, "version 82078 variant %d\n", cmd.rxcmd[0] >> 5);
    return FDC_82078_UNKN;
}

static state fdd_detect(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    struct floppy_cmd cmd;

    cmd.fdd = fdd;
    cmd.type = CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.txcmd[0] = FLOPPY_CMD_READ_ID;
    cmd.txcmd[1] = fdd->port;

    fdc_out(fdc, FLOPPY_DIR, FLOPPY_DSR_RATE_250K);
    return floppy_transmit(fdc, &cmd);
}

// static state pio_read(int fdc, int dev, void *buffer, unsigned int lba , unsigned int len)
// {
//     struct floppy_cmd cmd;
//     uint16_t cyl, head, sector;
//     uint16_t limit, end;

//     do {
//         lba2chs(lba, &cyl, &head, &sector);

//         /* Calculate end of transfer sector */
//         end = sector + len - 1;
//         end = min(16, end);
//         limit = end - sector + 1;

//         cmd.txlen = 9;
//         cmd.txcmd[0] = FLOPPY_CMD_READ_DATA;    /* 0 head number << 2 | drive number */
//         cmd.txcmd[1] = (head << 2) | dev;       /* 1 head number << 2 | drive number */
//         cmd.txcmd[2] = cyl;                     /* 2 cylinder number */
//         cmd.txcmd[3] = head;                    /* 3 head number */
//         cmd.txcmd[4] = sector;                  /* 4 starting sector number */
//         cmd.txcmd[5] = FLOPPY_SECTOR_SIZE_512;  /* 5 all floppy drives use 512bytes per sector */

//         floppy_transmit(fdc, &cmd);

//         len -= limit;
//         lba += limit;
//     } while(len);

//     return -ENOERR;
// }

static state floppy_enqueue(struct block_device *blk, struct block_request *req)
{
    // struct fdd_device *fdd = block_to_fdd(blk);

    spin_lock(&floppy_lock);



    spin_unlock(&floppy_lock);
    return -ENOERR;
}

static struct block_ops floppy_ops = {
    .enqueue = floppy_enqueue,
};

static state floppy_probe(struct platform_device *pdev, void *pdata)
{
    struct fdc_device *fdc;
    struct fdd_device *fdd;
    unsigned int count, offline;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    fdc = dev_kzalloc(&pdev->dev, sizeof(*fdc), GFP_KERNEL);
    if (!fdc)
        return -ENOMEM;

    fdc->dev = &pdev->dev;
    fdc->base = platform_resource_start(pdev, 0);
    platform_set_devdata(pdev, fdc);

    fdc->version = fdc_version(fdc);
    if (!fdc->version)
        return -ENODEV;

    fdc->irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (!fdc->irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -EINVAL;
    }
    irqchip_pass(fdc->irqchip);
    irq_request(FLOPPY_IRQ, 0, floppy_interrupt, NULL, DRIVER_NAME);

    for (count = 0; count < DRIVER_PER_FDC; ++count) {
        fdd = &fdc->fdd[count];

        fdd->host = fdc;
        fdd->port = count;
        fdd->block.ops = &floppy_ops;

        if (fdd_detect(fdd)) {
            offline++;
            continue;
        }

        dev_debug(&pdev->dev, "detected port %d\n", count);
        block_device_register(&fdd->block);
    } if (offline == DRIVER_PER_FDC) {
        dev_info(&pdev->dev, "no floppy disk driver found");
        goto error;
    }

    return -ENOERR;

error:
    irqchip_channel_release(fdc->irqchip);
    return -ENOMEM; /* auto memory release */
}

static const struct dt_device_id floppy_ids[] = {
    { .compatible = "ibm,generic-floppy" },
    { }, /* NULL */
};

static struct platform_driver floppy_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = floppy_ids,
    .probe = floppy_probe,
};

static state floppy_init(void)
{
    return platform_driver_register(&floppy_driver);
}
driver_initcall(floppy_init);
