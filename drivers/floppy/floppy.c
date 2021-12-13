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

#define FD_MAX_CMD_SIZE     16
#define FD_MAX_REPLY_SIZE   16
#define DRIVER_PER_FDC      2
#define FD_RESIDUE          5

struct floppy_info {
    unsigned int size;      /* nr of sectors total  */
    unsigned int cylinder;  /* nr of cylinder       */
    unsigned int head;      /* head per cylinder    */
    unsigned int sector;    /* sectors per cylinder */
    unsigned char gap1_size;
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
    uint8_t type;               /* transmit type    */
    unsigned char txlen;        /* length to send   */
    unsigned char rxlen;        /* length to reply  */
    unsigned char replen;       /* actual reply     */
    unsigned char retry;        /* retry count      */
    uint8_t txcmd[FD_MAX_CMD_SIZE];
    uint8_t rxcmd[FD_MAX_REPLY_SIZE];

    /* transmit ignored */
    struct fdd_device *fdd;     /* which device     */
    bool write;                 /* read or write    */
    char *buffer;               /* buffer pointer   */
    size_t buffersize;          /* buffer len       */
};

static SPIN_LOCK(floppy_lock);
static struct fdc_device *current_fdc;

static struct floppy_info floppy_type[] = {{
    },{ /* 1 - 360KB PC */
        .size =  720, .cylinder = 40, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_300K, .gap1_size = 0x2a,
    },{ /* 2 - 1.2MB AT */
        .size = 2400, .cylinder = 80, .head = 2, .sector = 15,
        .rate = FLOPPY_DSR_RATE_500K, .gap1_size = 0x1b,
    },{ /* 3 - 720KB AT */
        .size =  1440, .cylinder = 80, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x23,
    },{ /* 4 - 1.44MB 3.5 */
        .size =  2880, .cylinder = 80, .head = 2, .sector =  18,
        .rate = FLOPPY_DSR_RATE_500K, .gap1_size = 0x1b,
    },{ /* 5 - 2.88MB 3.5 */
        .size =  5760, .cylinder = 80, .head = 2, .sector =  36,
        .rate = FLOPPY_DSR_RATE_1M, .gap1_size = 0x1b,
    },{ /* 6 - 160KB AT */
        .size =  720, .cylinder = 40, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x1b,
    },{ /* 7 - 180KB AT */
        .size =  360, .cylinder = 40, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_300K, .gap1_size = 0x1b,
    },{ /* 8 - 320KB AT */
        .size =  640, .cylinder = 40, .head = 1, .sector =  9,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x1b,
    },
};

#ifdef CONFIG_ARCH_X86

#include <driver/rtc/qemu.h>
#define RTC_BASE  0x70

static void qemu_fdd_type(struct fdd_device *fdd)
{
    unsigned char index;

    outb(RTC_BASE, MC146818_FLOPPY_DRIVE_TYPE);
    index = inb(RTC_BASE + 1);
    index = (index >> (fdd->port ? 4 : 0)) & 0x0f;

    if (!index || index > ARRAY_SIZE(floppy_type))
        return;

    fdd->type = &floppy_type[index];
}

#endif  /* CONFIG_ARCH_X86 */

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
    uint8_t val, residue = FD_RESIDUE;
    state ret = -ENOERR;
    cmd->retry = -1;

retry:
    if (residue-- != FD_RESIDUE)
        fdc_reset(fdc);

    if (!residue) {
        dev_notice(fdc->dev, "abort transmit after five attempts\n");
        return -EBUSY;
    }

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
            dev_notice(fdc->dev, "timeout while send data\n");
            goto retry;
        }

        fdc_out(fdc, FLOPPY_FIFO, cmd->txcmd[count]);
    }

    if (cmd->type & CMD_PIO) /* not a pio command */
        if (!(fdc_in(fdc, FLOPPY_MSR) & FLOPPY_MSR_NDMA))
            goto out;

wait:
    /* wait for command to complete */
    if (cmd->type & CMD_WAIT_IRQ) {
        if ((ret = floppy_wait_irq(fdc))) {
            dev_notice(fdc->dev, "timeout while wait handle\n");
            goto retry;
        }
    }

    if (cmd->type & CMD_DMA) {

    } else if (cmd->type & CMD_PIO) {
        timeout = 50;

        while (--timeout) {
            val = fdc_in(fdc, FLOPPY_MSR) &
                       (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA);
            if (val == (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA))
                break;
            mdelay(1);
        } if (!timeout) {
            dev_notice(fdc->dev, "timeout while transfer data\n");
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
    /* get response from the fdc */
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
            goto retry;
        }

        if ((val & ~FLOPPY_MSR_CB) == FLOPPY_MSR_RQM)
            break;

        if (val == (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_CB))
            cmd->rxcmd[cmd->replen++] = fdc_in(fdc, FLOPPY_FIFO);
        else
            goto retry;
    }

    val = fdc_in(fdc, FLOPPY_MSR) & (FLOPPY_MSR_RQM |
        FLOPPY_MSR_DIO | FLOPPY_MSR_NDMA | FLOPPY_MSR_CB);
    if (FLOPPY_MSR_RQM != val)
        goto retry;

    return ret;
}

static inline state fdc_enable(struct fdc_device *fdc)
{
    unsigned int count = 4;
    struct floppy_cmd cmd;
    state ret;

    dev_info(fdc->dev, "enable controller\n");

    dor_mask(fdc, FLOPPY_DOR_RESET, FLOPPY_DOR_IRQ);
    udelay(20);
    dor_mask(fdc, 0, FLOPPY_DOR_RESET | FLOPPY_DOR_IRQ);

    /*
     * After the interrupt is received, send 4 SENSE INTERRUPT commands to
     * clear the interrupt status for each of the four logical drives,
     * supported by the controller.
     * See section 7.4 - "Drive Polling" of the Intel 82077AA datasheet for
     * a more detailed description of why this voodoo needs to be done.
     * Without this, initialization fails on real controllers (but still works
     * in QEMU)
     */

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_SENSE_INTERRUPT;

    while (count--) {
        ret = floppy_transmit(fdc, &cmd);
        if (ret) {
            dev_err(fdc->dev, "failed to enable controller\n");
            return ret;
        }
    }

    return -ENOERR;
}

static enum fdc_version fdc_version(struct fdc_device *fdc)
{
    struct floppy_cmd cmd;
    state ret;

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_DUMPREG;
    ret = floppy_transmit(fdc, &cmd);

    if (ret || (cmd.replen <= 0))
        return FDC_NONE;

    if ((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        dev_info(fdc->dev, "version 8272A\n");
        return FDC_8272A;
    }

    if (cmd.replen != 10) {
        dev_info(fdc->dev, "dumpregs: return of %d bytes.\n", cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_PERPENDICULAR_MODE;
    ret = floppy_transmit(fdc, &cmd);

    if (!ret) {
        dev_info(fdc->dev, "version 82072A\n");
        return FDC_82072A;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_LOCK;
    ret = floppy_transmit(fdc, &cmd);

    if ((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        dev_info(fdc->dev, "version pre 82077\n");
        return FDC_82077_ORIG;
    }

    if ((cmd.replen != 1) && (cmd.rxcmd[0] != 0x00)) {
        dev_info(fdc->dev, "unlock: return of %d bytes.\n", cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_PARTID;
    ret = floppy_transmit(fdc, &cmd);

    if (cmd.replen != 1) {
        dev_info(fdc->dev, "partid: return of %d bytes.\n", cmd.replen);
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
    state ret;

    memset(&cmd, 0, sizeof(cmd));
    cmd.type = CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.fdd = fdd;
    cmd.txlen = 2;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_READ_ID;
    cmd.txcmd[1] = fdd->port;

    fdc_out(fdc, FLOPPY_DIR, FLOPPY_DSR_RATE_250K);
    ret = floppy_transmit(fdc, &cmd);
    if (ret)
        return ret;

    return -ENOERR;
}

static state floppy_enqueue(struct block_device *blk, struct block_request *req)
{
    struct fdd_device *fdd = block_to_fdd(blk);
    struct fdc_device *fdc = fdd->host;
    unsigned int mcylinder, mhead, msector;
    unsigned int cylinder, head, sector;
    struct floppy_cmd cmd = { };

    if (fdd->type == &floppy_type[0])
        return -ENOMEDIUM;

    msector   = fdd->type->sector;
    mhead     = fdd->type->head;
    mcylinder = fdd->type->cylinder;

    sector    = (req->sector % msector) + 1;
    head      = (req->sector / msector) % (mhead + 1);
    cylinder  = ((req->sector / msector) / (mhead + 1)) % (mcylinder + 1);

    cmd.type     = CMD_PIO | CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.fdd      = fdd;
    cmd.txlen    = 9;
    cmd.rxlen    = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_READ_DATA;
    cmd.txcmd[5] = FLOPPY_SECTOR_SIZE_512;
    cmd.txcmd[6] = fdd->type->gap1_size;
    cmd.txcmd[7] = 0xff;

    spin_lock(&floppy_lock);

    cmd.txcmd[1] = (head << 2) | fdd->port;
    cmd.txcmd[2] = cylinder;
    cmd.txcmd[3] = head;
    cmd.txcmd[4] = sector;
    cmd.txcmd[6] = sector + req->length;
    cmd.buffer = req->buffer;
    cmd.buffersize = req->length * 512;
    floppy_transmit(fdc, &cmd);

    spin_unlock(&floppy_lock);
    return -ENOERR;
}

static struct block_ops floppy_ops = {
    .enqueue = floppy_enqueue,
};

static state floppy_probe(struct platform_device *pdev, const void *pdata)
{
    struct fdc_device *fdc;
    struct fdd_device *fdd;
    unsigned int count, offline;
    state ret;

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

    /* request irq handle */
    irq_request(FLOPPY_IRQ, 0, floppy_interrupt, NULL, DRIVER_NAME);

    fdc->irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (!fdc->irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -EINVAL;
    }
    irqchip_pass(fdc->irqchip);

    if ((ret = fdc_enable(fdc)))
        goto error;

    for (count = 0; count < DRIVER_PER_FDC; ++count) {
        fdd = &fdc->fdd[count];

        fdd->host = fdc;
        fdd->port = count;
        fdd->type = &floppy_type[0];

        if (fdd_detect(fdd)) {
            offline++;
            continue;
        }

        qemu_fdd_type(fdd);

        dev_debug(&pdev->dev, "detected port %d\n", count);
        fdd->block.dev = &pdev->dev;
        fdd->block.ops = &floppy_ops;
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
