/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

/**
 * TODO: Not tested on real hardware!!
 */

#define DRIVER_NAME "floppy"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <bits.h>
#include <mm.h>
#include <sleep.h>
#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/block.h>
#include <driver/floppy/floppy.h>
#include <printk.h>

#include <asm/io.h>
#include <asm/proc.h>

#define FLOPPY_SIZE_CODE    0x02    // 512 byte sectors

struct fdc_info {
    uint16_t base;
    int version;
};

struct floppy_cmd {
    uint8_t type;               /* transmission type */

    unsigned char txlen;        /* Length to send */
    unsigned char rxlen;        /* Length to reply */
    unsigned char replen;       /* Actual reply */
    char retry;                 /* retry count */
    uint8_t txcmd[FD_MAX_CMD_SIZE];
    uint8_t rxcmd[FD_MAX_REPLY_SIZE];

    /* CMD_TRANSMIT ignored */
    char dev;           /* Floppy drive select */
    char write;         /* Read or write */
    char *buffer;       /* Buffer pointer */
    size_t buffersize;  /* Buffer len */
};

static const uint16_t fdc_base[FDC_NR] = {FDC1_BASE, FDC2_BASE};
static struct fdc_info fdc_info[FDC_NR];
static struct platform_device floppy_device[FDC_NR][DRIVE_NR];

static __always_inline
uint8_t fdc_inb(int fdc, int reg)
{
    return inb(fdc_info[fdc].base + reg);
}

static __always_inline
void fdc_outb(int fdc, int reg, uint8_t value)
{
    outb(fdc_info[fdc].base + reg, value);
}

static inline void driver_select(int fdc, int dev)
{
    uint8_t val = fdc_inb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER);
    val &= ~FLOPPY_DOR_DSEL_MASK;
    val |= dev & FLOPPY_DOR_DSEL_MASK;
    fdc_outb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER, val);
}

static inline void motor_power(int fdc, int dev, bool power)
{
    uint8_t val = fdc_inb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER);

    if(!!(val & (FLOPPY_DOR_MOTOR_A << dev)) == power)
        return;

    if(power) {
        fdc_outb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER,
                val | (FLOPPY_DOR_MOTOR_A << dev));
        msleep(300); /* wait for stability */
    } else {
        fdc_outb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER,
                val & ~(FLOPPY_DOR_MOTOR_A << dev));
    }
}

static void fdc_reset(int fdc)
{
    uint8_t val;

    if(fdc_info[fdc].version >= FDC_82072A) {
        val = fdc_inb(fdc, FLOPPY_DATARATE_SELECT_REGISTER);
        val |= FLOPPY_DSR_SWRST;
        fdc_outb(fdc, FLOPPY_DATARATE_SELECT_REGISTER, val);
    } else {
        val = fdc_inb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER);
        fdc_outb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER, val & ~FLOPPY_DOR_RESET);
        udelay(4);
        fdc_outb(fdc, FLOPPY_DIGITAL_OUTPUT_REGISTER, val);
    }
}

static bool irq_occur;
static irqreturn_t floppy_interrupt(irqnr_t intnr, void *fdc)
{
    irq_occur = 1;
    return IRQ_RET_HANDLED;
}

static int wait_irq(void)
{
    unsigned int timeout = 0x10000000;
    while(!irq_occur && --timeout)
        msleep(1);
    return !timeout;
}

#define CMD_WAIT_IRQ    BIT(0)
#define CMD_DIRVER      BIT(1)
#define CMD_PIO         BIT(2)
#define CMD_DMA         BIT(3)

static state floppy_transmit(int fdc, struct floppy_cmd *cmd)
{
    uint8_t val, timeout, residue = 5;
    unsigned int count;
    state ret = -ENOERR;
    cmd->retry = -1;

retry:
    if(!residue--)
        return -EBUSY;

    cmd->retry++;
    irq_occur = 0;

    if(cmd->type & CMD_DIRVER) {
        motor_power(fdc, cmd->dev, 1);
        driver_select(fdc, cmd->dev);
    }

    if(cmd->type & CMD_DMA) {

    }

    for(count = 0; cmd->txlen != count; ++count) {
        timeout = 50;

        while(--timeout) {
            val = fdc_inb(fdc, FLOPPY_MAIN_STATUS_REGISTER) &
                (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO);
            if(val == FLOPPY_MSR_RQM)
                break;
            msleep(1);
        }
        if(!timeout) {
            fdc_reset(fdc);
            goto retry;
        }

        fdc_outb(fdc, FLOPPY_DATA_FIFO, cmd->txcmd[count]);
    }

    if(cmd->type & CMD_PIO)
        if(!(fdc_inb(fdc, FLOPPY_MAIN_STATUS_REGISTER) & FLOPPY_MSR_NDMA))
            goto out;

wait:
    if(cmd->type & CMD_WAIT_IRQ) {
        val = wait_irq();
        if(val) {
            fdc_reset(fdc);
            goto retry;
        }
    }

    if(cmd->type & CMD_PIO) {
        while(--timeout) {
            val = fdc_inb(fdc, FLOPPY_MAIN_STATUS_REGISTER) &
                (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA);
            if(val == (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA))
                break;
            msleep(1);
        }
        if(!timeout) {
            fdc_reset(fdc);
            goto retry;
        }

        if(cmd->buffersize) {
            if(cmd->write)
                fdc_outb(fdc, FLOPPY_DATA_FIFO, *cmd->buffer++);
            else
                *cmd->buffer++ = fdc_inb(fdc, FLOPPY_DATA_FIFO);
            cmd->buffersize--;
        } else {
            if(cmd->write)
                fdc_outb(fdc, FLOPPY_DATA_FIFO, 0x00);
            else
                fdc_inb(fdc, FLOPPY_DATA_FIFO);
            ret = -ENOMEM;
        }

        goto wait;
    }

out:
    while(cmd->rxlen != cmd->replen) {
        timeout = 50;

        while(--timeout) {
            val = fdc_inb(fdc, FLOPPY_MAIN_STATUS_REGISTER) &
                (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_NDMA | FLOPPY_MSR_CB);
            if(val & FLOPPY_MSR_RQM)
                break;
            msleep(1);
        }
        if(!timeout) {
            fdc_reset(fdc);
            goto retry;
        }

        if((val & ~FLOPPY_MSR_CB) == FLOPPY_MSR_RQM)
            break;
        if(val == (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_CB))
            cmd->rxcmd[cmd->replen++] = fdc_inb(fdc, FLOPPY_DATA_FIFO);
        else
            break;
    }

    val = fdc_inb(fdc, FLOPPY_MAIN_STATUS_REGISTER) &
    (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_NDMA | FLOPPY_MSR_CB);
    if(FLOPPY_MSR_RQM != val) {
        fdc_reset(fdc);
        goto retry;
    }

    return ret;
}

// static state pio_read(int fdc, int dev, void *buffer, unsigned int lba , unsigned int len)
// {
//     struct floppy_cmd cmd;
//     uint16_t cyl, head, sector;
//     uint16_t limit, end;

//     do{
//         lba2chs(lba, &cyl, &head, &sector);

//         /* Calculate end of transfer sector */
//         end = sector + len - 1;
//         end = min(16, end);
//         limit = end - sector + 1;

//         cmd.txlen = 9;
//         cmd.txcmd[0] = FLOPPY_COMMAND_READ_DATA;    /* 0 head number << 2 | drive number */
//         cmd.txcmd[1] = (head << 2) | dev;           /* 1 head number << 2 | drive number */
//         cmd.txcmd[2] = cyl;                         /* 2 cylinder number */
//         cmd.txcmd[3] = head;                        /* 3 head number */
//         cmd.txcmd[4] = sector;                      /* 4 starting sector number */
//         cmd.txcmd[5] = FLOPPY_SIZE_CODE;            /* 5 all floppy drives use 512bytes per sector */

//         floppy_transmit(fdc, &cmd);

//         len -= limit;
//         lba += limit;
//     }while(len);

//     return OK;
// }

static enum floppy_version floppy_version(int fdc)
{
    struct floppy_cmd cmd;
    state ret;

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_COMMAND_DUMPREG;
    ret = floppy_transmit(fdc, &cmd);

    if(ret || (cmd.replen <= 0))
        return FDC_NONE;

    if((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        pr_info("FDC%d version 8272A\n", fdc);
        return FDC_8272A;
    }

    if(cmd.replen != 10) {
        pr_info("FDC%d DUMPREGS: return of %d bytes.\n", fdc, cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_COMMAND_PERPENDICULAR_MODE;
    ret = floppy_transmit(fdc, &cmd);

    if(!ret) {
        pr_info("FDC%d version 82072A\n", fdc);
        return FDC_82072A;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_COMMAND_LOCK;
    ret = floppy_transmit(fdc, &cmd);

    if((cmd.replen == 1) && (cmd.rxcmd[0] == 0x80)) {
        pr_info("FDC%d version pre 82077\n", fdc);
        return FDC_82077_ORIG;
    }

    if((cmd.replen != 1) && (cmd.rxcmd[0] != 0x00)) {
        pr_info("FDC%d UNLOCK: return of %d bytes.\n", fdc, cmd.replen);
        return FDC_UNKNOWN;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.txlen = 1;
    cmd.rxlen = 16;
    cmd.txcmd[0] = FLOPPY_COMMAND_PARTID;
    ret = floppy_transmit(fdc, &cmd);

    if(cmd.replen != 1) {
        pr_info("FDC%d PARTID: return of %d bytes.\n", fdc, cmd.replen);
        return FDC_UNKNOWN;
    }

    if(cmd.rxcmd[0] == 0x80) {
        pr_info("FDC%d version post 82077\n", fdc);
        return FDC_82077;
    }

    switch(cmd.rxcmd[0] >> 5) {
        case 0x00:
            pr_info("FDC%d version 82078\n", fdc);
            return FDC_82078;
        case 0x01:
            pr_info("FDC%d version 44pin 82078\n", fdc);
            return FDC_82078;
        case 0x02:
            pr_info("FDC%d version S82078B\n", fdc);
            return FDC_S82078B;
        case 0x03:
            pr_info("FDC%d version PC87306\n", fdc);
            return FDC_82078;
    }

    pr_info("FDC%d version 82078 variant %d\n", fdc, cmd.rxcmd[0] >> 5);
	return FDC_82078_UNKN;
}

static state floppy_probe(struct platform_device *pdev)
{
    // struct block_device *bdev;

    // bdev = kmalloc(sizeof(*bdev), GFP_KERNEL);
    // // block_device_register(bdev);

    return -ENOERR;
}

static void floppy_remove(struct platform_device *pdev)
{
}

static struct platform_device_id floppy_id[] = {
    {.name = DRIVER_NAME},
    { }, /* NULL */
};

static struct platform_driver floppy_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = floppy_id,
    .probe = floppy_probe,
    .remove = floppy_remove,
};

static int floppy_scan_driver(int fdc)
{
    int driver = 0;

    /* Register device */
    floppy_device[fdc][driver].name = DRIVER_NAME;
    floppy_device[fdc][driver].id = driver;
    platform_device_register(&floppy_device[fdc][driver]);

    return -ENOERR;
}

static int floppy_scan_fdc(int fdc)
{
    int count, driver;

    fdc_info[fdc].base = fdc_base[fdc];

    irq_request(FLOPPY_IRQ, 0, floppy_interrupt, NULL, DRIVER_NAME);

    if(!(fdc_info[fdc].version = floppy_version(fdc)))
        return -ENODEV;

    for(count = 0; count < DRIVE_NR; ++count)
        if(floppy_scan_driver(count))
            driver++;

    if(!driver)
        return -ENODEV;

    return fdc;
}

static state floppy_init(void)
{
    int count, fdc;

    for(count = 0; count < FDC_NR; ++count)
        if(floppy_scan_fdc(count))
            fdc++;

    if(!fdc) {
        pr_info("no floppy controllers found\n");
        return -ENODEV;
    }

    platform_driver_register(&floppy_driver);

    return -ENOERR;
}
driver_initcall(floppy_init);
