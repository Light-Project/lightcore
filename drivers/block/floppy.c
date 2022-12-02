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
#include <delay.h>
#include <asm/proc.h>
#include <asm/io.h>

#define FD_MAX_CMD_SIZE     16
#define FD_MAX_REPLY_SIZE   16
#define DRIVER_PER_FDC      2
#define FD_RESIDUE          5

struct fdd_device {
    struct block_device block;
    struct fdc_device *host;
    const struct floppy_info *type;
    unsigned char port;
    unsigned int lastcyl;
};

#define block_to_fdd(blk) \
    container_of(blk, struct fdd_device, block)

struct fdc_device {
    struct fdd_device fdd[DRIVER_PER_FDC];
    struct irqchip_channel *irqchip;
    struct device *dev;

    resource_size_t base;
    enum fdc_version version;
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

struct floppy_info {
    const char *name;
    unsigned int size;          /* nr of sectors total  */
    unsigned int cylinder;      /* nr of cylinder       */
    unsigned int head;          /* head per cylinder    */
    unsigned int sector;        /* sectors per cylinder */
    unsigned char gap1_size;    /* */
    unsigned char track;        /* */
    unsigned char rate;         /* */
};

static const struct floppy_info floppy_type[] = {{
    },{ /* 1 - 360KB PC */
        .name = "360KB-PC",
        .size =  720, .cylinder = 40, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_300K, .gap1_size = 0x2a,
    },{ /* 2 - 1.2MB AT */
        .name = "1.2MB-AT",
        .size = 2400, .cylinder = 80, .head = 2, .sector = 15,
        .rate = FLOPPY_DSR_RATE_500K, .gap1_size = 0x1b,
    },{ /* 3 - 720KB AT */
        .name = "720KB-AT",
        .size =  1440, .cylinder = 80, .head = 2, .sector =  9,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x23,
    },{ /* 4 - 1.44MB 3.5 */
        .name = "1.44MB-3.5",
        .size =  2880, .cylinder = 80, .head = 2, .sector =  18,
        .rate = FLOPPY_DSR_RATE_500K, .gap1_size = 0x1b,
    },{ /* 5 - 2.88MB 3.5 */
        .name = "2.88MB-3.5",
        .size =  5760, .cylinder = 80, .head = 2, .sector =  36,
        .rate = FLOPPY_DSR_RATE_1M, .gap1_size = 0x1b,
    },{ /* 6 - 160KB 5.25 */
        .name = "160KB-5.25",
        .size =  720, .cylinder = 40, .head = 1, .sector =  8,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x1b,
    },{ /* 7 - 180KB 5.25 */
        .name = "180KB-5.25",
        .size =  360, .cylinder = 40, .head = 1, .sector =  9,
        .rate = FLOPPY_DSR_RATE_300K, .gap1_size = 0x1b,
    },{ /* 8 - 320KB 5.25 */
        .name = "320KB-5.25",
        .size =  640, .cylinder = 40, .head = 2, .sector =  8,
        .rate = FLOPPY_DSR_RATE_250K, .gap1_size = 0x1b,
    },
};

static SPIN_LOCK(floppy_lock);
static struct fdc_device *current_fdc;

#ifdef CONFIG_ARCH_X86
#include <driver/rtc/qemu.h>
#define RTC_BASE  0x70

static void qemu_fdd_type(struct fdd_device *fdd)
{
    unsigned char index;

    outb(RTC_BASE, MC146818_FLOPPY_DRIVE_TYPE);
    index = inb(RTC_BASE + 1);
    index = (index >> (fdd->port ? 0 : 4)) & 0x0f;

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
    fdc->dor = (fdc_in(fdc, FLOPPY_DOR) & ~off) | on;
    fdc_out(fdc, FLOPPY_DOR, fdc->dor);
    return fdc->dor;
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

static void motor_power(struct fdd_device *fdd, bool enable)
{
    struct fdc_device *fdc = fdd->host;

    if (!!(fdc->dor & (FLOPPY_DOR_MOTOR_A << fdd->port)) == enable)
        return;

    if (enable) {
        dor_mask(fdc, 0, FLOPPY_DOR_MOTOR_A << fdd->port);
        mdelay(50); /* wait for stability */
    } else
        dor_mask(fdc, FLOPPY_DOR_MOTOR_A << fdd->port, 0);
}

static void driver_select(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    dor_mask(fdc, FLOPPY_DOR_DSEL_MASK, fdd->port);
}

static void floppy_speed(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    fdc_out(fdc, FLOPPY_CCR, fdd->type->rate);
}

static state fdc_status_dump(struct fdc_device *fdc, uint8_t *status)
{
    state ret = -ENOERR;

    if (status[0] & ~(FLOPPY_ST0_INTR | FLOPPY_ST0_DS | FLOPPY_ST0_SE)) {
        dev_err(fdc->dev, "st0: error code (0x%02x)\n", status[0]);
        if (status[0] & FLOPPY_ST0_ECE) {
            dev_err(fdc->dev, "  equipment check error\n");
            ret = -ENODEV;
        }
        if (status[0] & FLOPPY_ST0_NR) {
            dev_err(fdc->dev, "  not ready\n");
            ret = -EINVAL;
        }
        if (status[0] & FLOPPY_ST0_HA) {
            dev_err(fdc->dev, "  head (address)\n");
            ret = -EINVAL;
        }
    }

    if (status[1]) {
        dev_warn(fdc->dev, "st1: error code (0x%02x)\n", status[1]);
        if (status[1] & FLOPPY_ST1_EOC) {
            dev_warn(fdc->dev, "  end of cylinder\n");
            ret = -EINVAL;
        }
        if (status[1] & FLOPPY_ST1_CRC) {
            dev_warn(fdc->dev, "  crc error in data or addr\n");
            ret = -EIO;
        }
        if (status[1] & FLOPPY_ST1_OR) {
            dev_warn(fdc->dev, "  overrun\n");
            ret = -EBUSY;
        }
        if (status[1] & FLOPPY_ST1_ND) {
            dev_warn(fdc->dev, "  no data - unreadable\n");
            ret = -ENODATA;
        }
        if (status[1] & FLOPPY_ST1_WP) {
            dev_warn(fdc->dev, "  write protect\n");
            ret = -EROFS;
        }
        if (status[1] & FLOPPY_ST1_MAM) {
            dev_warn(fdc->dev, "  missing address mark\n");
            ret = -EINVAL;
        }
    }

    if (status[2]) {
        dev_warn(fdc->dev, "st2: error code (0x%02x)\n", status[2]);
        if (status[2] & FLOPPY_ST2_CM) {
            dev_warn(fdc->dev, "  control mark = deleted\n");
            ret = -EHOSTDOWN;
        }
        if (status[2] & FLOPPY_ST2_CRC) {
            dev_warn(fdc->dev, "  crc error in data field\n");
            ret = -EIO;
        }
        if (status[2] & FLOPPY_ST2_WC) {
            dev_warn(fdc->dev, "  wrong cylinder\n");
            ret = -EINVAL;
        }
        if (status[2] & FLOPPY_ST2_SEH) {
            dev_warn(fdc->dev, "  scan equal hit\n");
            ret = -ENODATA;
        }
        if (status[2] & FLOPPY_ST2_SNS) {
            dev_warn(fdc->dev, "  scan not satisfied\n");
            ret = -ENODATA;
        }
        if (status[2] & FLOPPY_ST2_BC) {
            dev_warn(fdc->dev, "  bad cylinder");
            ret = -EFAULT;
        }
        if (status[2] & FLOPPY_ST2_MAM) {
            dev_warn(fdc->dev, "  missing address mark (again)\n");
            ret = -EINVAL;
        }
    }

    return ret;
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

    while (!fdc->irq_occur && --timeout)
        udelay(100);

    if (!timeout)
        return -EBUSY;

    fdc->irq_occur = false;
    return -ENOERR;
}

static state floppy_transmit(struct fdc_device *fdc, struct floppy_cmd *cmd)
{
    unsigned int timeout, count;
    uint8_t val, residue;
    state ret = -ENOERR;

    current_fdc = fdc;
    fdc->irq_occur = false;

    if (cmd->retry)
        residue = cmd->retry + 1;
    else
        residue = FD_RESIDUE;

retry:
    if (residue-- != FD_RESIDUE)
        fdc_reset(fdc);

    if (!residue) {
        dev_debug(fdc->dev, "abort transmit after several attempts\n");
        return -EBUSY;
    }

    if (cmd->type & CMD_DIRVER) {
        motor_power(cmd->fdd, true);
        driver_select(cmd->fdd);
        floppy_speed(cmd->fdd);
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
            dev_debug(fdc->dev, "timeout while send data\n");
            goto retry;
        }

        fdc_out(fdc, FLOPPY_FIFO, cmd->txcmd[count]);
    }

    if (cmd->type & CMD_PIO) /* not a pio command */
        if (!(fdc_in(fdc, FLOPPY_MSR) & FLOPPY_MSR_NDMA))
            goto out;

    /* wait for command to complete */
    if (cmd->type & CMD_WAIT_IRQ) {
        if ((ret = floppy_wait_irq(fdc))) {
            dev_debug(fdc->dev, "timeout while wait handle\n");
            goto retry;
        }
    }

    if (cmd->type & CMD_DMA) {

    } else if (cmd->type & CMD_PIO) {
        for (count = 0; ; count++) {
            val = fdc_in(fdc, FLOPPY_MSR) &
                (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA);
            if (val != (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA))
                break;

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
        }
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
            dev_debug(fdc->dev, "wait timeout while receive cmd\n");
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

    if (cmd->type & CMD_WAIT_IRQ && !cmd->rxlen) {
        struct floppy_cmd cmd;

        memset(&cmd, 0, sizeof(cmd));
        cmd.txlen = 1;
        cmd.rxlen = FD_MAX_REPLY_SIZE;
        cmd.txcmd[0] = FLOPPY_CMD_SENSE_INTERRUPT;

        ret = floppy_transmit(fdc, &cmd);
        if (ret)
            dev_debug(fdc->dev, "failed to check irq after cmd\n");

        /* clear irrelevant registers */
        cmd.rxcmd[1] = 0x00;
        cmd.rxcmd[2] = 0x00;
        ret = fdc_status_dump(fdc, cmd.rxcmd);
    }

    return ret;
}

static inline state fdc_enable(struct fdc_device *fdc)
{
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

    ret = floppy_transmit(fdc, &cmd);
    if (ret)
        dev_err(fdc->dev, "failed to enable controller\n");

    cmd.rxcmd[1] = 0;
    cmd.rxcmd[2] = 0;
    return fdc_status_dump(fdc, cmd.rxcmd);
}

static enum fdc_version fdc_version(struct fdc_device *fdc)
{
    struct floppy_cmd cmd;
    state ret;

    memset(&cmd, 0, sizeof(cmd));
    cmd.retry = 1;
    cmd.txlen = 1;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_DUMPREG;
    ret = floppy_transmit(fdc, &cmd);

    if (ret || !cmd.replen)
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
    struct floppy_cmd cmd = {};
    state ret;

    cmd.type = CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.fdd = fdd;
    cmd.txlen = 2;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.txcmd[0] = FLOPPY_CMD_READ_ID;
    cmd.txcmd[1] = fdd->port;

    fdc_out(fdc, FLOPPY_CCR, FLOPPY_DSR_RATE_250K);
    ret = floppy_transmit(fdc, &cmd);
    if (ret) {
        return ret;
    }

    if (cmd.rxcmd[0] & FLOPPY_ST0_INTR)
        return -ENOMEDIUM;

    return -ENOERR;
}

static state fdd_specify(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    struct floppy_cmd cmd = {};

    cmd.txlen = 3;
    cmd.txcmd[0] = FLOPPY_CMD_SPECIFY;
    cmd.txcmd[1] = 0xaf;
    cmd.txcmd[2] = 0x02 | FLOPPY_SPECIFY2_NODMA;

    return floppy_transmit(fdc, &cmd);
}

static inline state fdd_recalibrate(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    struct floppy_cmd cmd = {};

    cmd.type = CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.fdd = fdd;
    cmd.txlen = 2;
    cmd.txcmd[0] = FLOPPY_CMD_RECALIBRATE;
    cmd.txcmd[1] = fdd->port;

    return floppy_transmit(fdc, &cmd);
}

static inline state fdd_perpendicular(struct fdd_device *fdd)
{
    struct fdc_device *fdc = fdd->host;
    struct floppy_cmd cmd = {};

    cmd.txlen = 2;
    cmd.txcmd[0] = FLOPPY_CMD_PERPENDICULAR_MODE;
    cmd.txcmd[1] = 0x40 << fdd->port;

    return floppy_transmit(fdc, &cmd);
}

static state fdd_seek(struct fdd_device *fdd, unsigned int cylinder)
{
    struct fdc_device *fdc = fdd->host;
    struct floppy_cmd cmd = {};
    state ret;

    if (fdd->lastcyl == cylinder)
        return -ENOERR;

    cmd.type = CMD_DIRVER | CMD_WAIT_IRQ;
    cmd.fdd = fdd;
    cmd.txlen = 3;
    cmd.txcmd[0] = FLOPPY_CMD_SEEK;
    cmd.txcmd[1] = fdd->port;
    cmd.txcmd[2] = cylinder;

    ret = floppy_transmit(fdc, &cmd);
    if (ret)
        return ret;

    fdd->lastcyl = cylinder;
    return -ENOERR;
}

static state floppy_enqueue(struct block_device *blk, struct block_request *req)
{
    struct fdd_device *fdd = block_to_fdd(blk);
    struct fdc_device *fdc = fdd->host;
    unsigned int mcylinder, mhead, msector;
    unsigned int cylinder, head, sector;
    struct floppy_cmd cmd = {};
    state ret;

    if (fdd->type == &floppy_type[0])
        return -ENOMEDIUM;

    msector   = fdd->type->sector;
    mhead     = fdd->type->head;
    mcylinder = fdd->type->cylinder;

    sector    = (req->sector % msector) + 1;
    head      = (req->sector / msector) % (mhead + 1);
    cylinder  = ((req->sector / msector) / (mhead + 1)) % (mcylinder + 1);

    cmd.type  = CMD_DIRVER | CMD_WAIT_IRQ | CMD_PIO;
    cmd.fdd   = fdd;
    cmd.txlen = 9;
    cmd.rxlen = FD_MAX_REPLY_SIZE;
    cmd.buffer = req->buffer;
    cmd.buffersize = req->length * 512;

    cmd.txcmd[0] = FLOPPY_CMD_READ_DATA;        /* Transfer Command */
    cmd.txcmd[1] = (head << 2) | fdd->port;     /* HD:US1:US0 = Head and drive */
    cmd.txcmd[2] = cylinder;                    /* Cylinder */
    cmd.txcmd[3] = head;                        /* First head (should match with above) */
    cmd.txcmd[4] = sector;                      /* First sector, strangely counts from 1 */
    cmd.txcmd[5] = FLOPPY_SECTOR_SIZE_512;      /* Bytes/Sector, 128*2^x (x=2 -> 512) */
    cmd.txcmd[6] = sector + req->length - 1;    /* Number of tracks to operate on */
    cmd.txcmd[7] = fdd->type->gap1_size;        /* GAP3 length, 27 is default for 3.5" */
    cmd.txcmd[8] = 0xff;                        /* Data length (0xff if B/S != 0) */

    spin_lock(&floppy_lock);

    fdd_perpendicular(fdd);

    if ((ret = fdd_recalibrate(fdd))) {
        dev_err(fdc->dev, "transfer recalibrate error\n");
        goto error;
    }

    if ((ret = fdd_specify(fdd))) {
        dev_err(fdc->dev, "transfer specify error\n");
        goto error;
    }

    if ((ret = fdd_seek(fdd, cylinder))) {
        dev_err(fdc->dev, "transfer seek error\n");
        goto error;
    }

    if ((ret = floppy_transmit(fdc, &cmd))) {
        dev_err(fdc->dev, "transfer error\n");
        goto error;
    }

    spin_unlock(&floppy_lock);
    return fdc_status_dump(fdc, cmd.rxcmd);

error:
    spin_unlock(&floppy_lock);
    return ret;
}

static struct block_ops floppy_ops = {
    .enqueue = floppy_enqueue,
};

static state floppy_probe(struct platform_device *pdev, const void *pdata)
{
    struct fdc_device *fdc;
    unsigned int count, offline = 0;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENXIO;

    fdc = platform_kzalloc(pdev, sizeof(*fdc), GFP_KERNEL);
    if (!fdc)
        return -ENOMEM;

    fdc->irqchip = dt_irqchip_channel(pdev->dt_node, 0);
    if (!fdc->irqchip) {
        platform_err(pdev, "unable to request irq\n");
        return -EINVAL;
    }

    fdc->dev = &pdev->dev;
    fdc->base = platform_resource_start(pdev, 0);
    platform_set_devdata(pdev, fdc);

    fdc->version = fdc_version(fdc);
    if (!fdc->version) {
        ret = -ENODEV;
        goto error;
    }

    irq_request(FLOPPY_IRQ, 0, floppy_interrupt, NULL, DRIVER_NAME);
    irqchip_pass(fdc->irqchip);

    if ((ret = fdc_enable(fdc)))
        goto error;

    for (count = 0; count < DRIVER_PER_FDC; ++count) {
        struct fdd_device *fdd;

        fdd = &fdc->fdd[count];
        fdd->host = fdc;
        fdd->port = count;
        fdd->type = &floppy_type[0];

        qemu_fdd_type(fdd);
        if (!fdd->type->name) {
            offline++;
            continue;
        }

        if (fdd_detect(fdd)) {
            offline++;
            continue;
        }

        platform_info(pdev, "detected port%d version %s\n", count, fdd->type->name);

        fdd->block.dev = &pdev->dev;
        fdd->block.ops = &floppy_ops;
        block_device_register(&fdd->block);
    } if (offline == DRIVER_PER_FDC) {
        platform_info(pdev, "no floppy disk driver found\n");
        ret = -ENOMEDIUM;
        goto error;
    }

    return -ENOERR;

error:
    irqchip_channel_release(fdc->irqchip);
    return ret; /* auto memory release */
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
