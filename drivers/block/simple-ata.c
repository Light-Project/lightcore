/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ata-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <driver/block.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/ata.h>
#include <printk.h>

#include <asm/delay.h>
#include <asm/proc.h>
#include <asm/io.h>

#define ATASIM_NR       2
#define ATASIM_PORTS    2
#define ATASIM_BLOCKSZ  512
#define ATASIM_TIMEOUT  32000

struct atasim_host {
    struct spinlock lock;
    resource_size_t base;
};

struct atasim_device {
    struct block_device block;
    struct atasim_host *host;
    unsigned char port;
};

struct atasim_cmd {
    uint8_t feature;
    uint8_t sector_count;
    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;
    uint8_t device;
    uint8_t command;

    uint8_t feature2;
    uint8_t sector_count2;
    uint8_t lba_low2;
    uint8_t lba_mid2;
    uint8_t lba_high2;
};

#define block_to_atasim(bdev) \
    container_of(bdev, struct atasim_device, block);

static __always_inline uint8_t
atasim_inb(struct atasim_device *atasim, int reg)
{
    struct atasim_host *host = atasim->host;
    return inb(host->base + reg);
}

static __always_inline uint16_t
atasim_inw(struct atasim_device *atasim, int reg)
{
    struct atasim_host *host = atasim->host;
    return inw(host->base + reg);
}

static __always_inline void
atasim_outb(struct atasim_device *atasim, int reg, uint8_t val)
{
    struct atasim_host *host = atasim->host;
    outb(host->base + reg, val);
}

static __always_inline void
atasim_outw(struct atasim_device *atasim, int reg, uint16_t val)
{
    struct atasim_host *host = atasim->host;
    outw(host->base + reg, val);
}

static bool atasim_wait(struct atasim_device *atasim, uint8_t mask,
                        int8_t pass, unsigned int timeout)
{
    uint8_t val;

    while (--timeout) {
        val = atasim_inb(atasim, ATA_REG_STATUS);
        if ((val & mask) == pass)
            break;
        udelay(1000);
    }

    return !!timeout;
}

/**
 * atasim_wait_ready - Wait for the device to be ready
 * @atasim: device to wait
 */
static bool atasim_wait_ready(struct atasim_device *atasim)
{
    return atasim_wait(atasim, ATA_STATUS_DRDY, ATA_STATUS_DRDY, ATASIM_TIMEOUT);
}

/**
 * atasim_wait_busy - Wait for the device to be not-busy
 * @atasim: device to wait
 */
static bool atasim_wait_busy(struct atasim_device *atasim)
{
    return atasim_wait(atasim, ATA_STATUS_BSY, 0, ATASIM_TIMEOUT);
}

/**
 * atasim_pause_wait_busy - Pauses for one ata cycle first
 * @atasim: device to wait
 */
static bool atasim_pause_wait_busy(struct atasim_device *atasim)
{
    /* Wait one PIO transfer cycle */
    atasim_inb(atasim, ATA_REG_ASTAT);
    return atasim_wait_busy(atasim);
}

static uint8_t atasmi_devsel(struct atasim_device *atasim, uint8_t device)
{
    device |= ATA_DEVSEL_OBS;

    if (atasim->port)
        device |= ATA_DEVSEL_DEV;

    atasim_outb(atasim, ATA_REG_DEVSEL, device);
    return device;
}

static void atasim_reset(struct atasim_device *atasim)
{
    uint32_t timeout = ATASIM_TIMEOUT;
    uint8_t val;

    /* Pulse SRST */
    atasim_outb(atasim, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN | ATA_DEVCTL_SRST);
    udelay(5);
    atasim_outb(atasim, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN);
    udelay(2000);

    /* wait for device to become not busy */
    if (!atasim_wait_busy(atasim))
        goto done;

    if (atasim->port){
        while (--timeout) {
            /* Change device */
            val = atasmi_devsel(atasim, 0);
            if (!atasim_wait_busy(atasim))
                goto done;
            if (atasim_inb(atasim, ATA_REG_DEVSEL) == val)
                break;
            udelay(1000);
        }
    } else {
        atasim_outb(atasim, ATA_REG_DEVSEL, ATA_DEVSEL_OBS);
    }

    /* On a user-reset request, wait for RDY if it is an ATA device. */
    atasim_wait_ready(atasim);

done:
    /* Enable interrupts */
    atasim_outb(atasim, ATA_REG_DEVCTL, ATA_DEVCTL_HD15);
}

static state atasim_cmd(struct atasim_device *atasim, struct atasim_cmd *cmd)
{
    if (!atasim_wait_busy(atasim))
        return -EIO;

    /* Select device */
    atasmi_devsel(atasim, cmd->device);
    if (!atasim_wait_busy(atasim))
        return -EIO;

    /* Check for ATA_CMD_(READ|WRITE)_(SECTORS|DMA)_EXT commands. */
    if ((cmd->command & ~0x11) == ATA_CMD_READ_SECTORS_EXT) {
        atasim_outb(atasim, ATA_REG_FEATURE, cmd->feature2);
        atasim_outb(atasim, ATA_REG_NSECT, cmd->sector_count2);
        atasim_outb(atasim, ATA_REG_LBAL, cmd->lba_low2);
        atasim_outb(atasim, ATA_REG_LBAM, cmd->lba_mid2);
        atasim_outb(atasim, ATA_REG_LBAH, cmd->lba_high2);
    }

    atasim_outb(atasim, ATA_REG_FEATURE, cmd->feature);
    atasim_outb(atasim, ATA_REG_NSECT, cmd->sector_count);
    atasim_outb(atasim, ATA_REG_LBAL, cmd->lba_low);
    atasim_outb(atasim, ATA_REG_LBAM, cmd->lba_mid);
    atasim_outb(atasim, ATA_REG_LBAH, cmd->lba_high);
    atasim_outb(atasim, ATA_REG_CMD, cmd->command);

    return -ENOERR;
}

static state atasim_transfer(struct atasim_device *atasim, void *buffer,
                             unsigned int count, bool iswrite)
{
    uint8_t val;

    for (;;) {
        if (iswrite) { /* Write data to controller */
#ifdef CONFIG_BLK_ATASIM32
            outsl(atasim->host->base, buffer, ATASIM_BLOCKSZ / 4);
#else
            outsw(atasim->host->base, buffer, ATASIM_BLOCKSZ / 2);
#endif
        } else { /* Read data to controller */
#ifdef CONFIG_BLK_ATASIM32
            insl(atasim->host->base, buffer, ATASIM_BLOCKSZ / 4);
#else
            insw(atasim->host->base, buffer, ATASIM_BLOCKSZ / 2);
#endif
        }

        if (!atasim_pause_wait_busy(atasim))
            return -EIO;

        if (!count--)
            break;

        val = atasim_inb(atasim, ATA_REG_STATUS);
        val &= (ATA_STATUS_BSY | ATA_STATUS_DRQ | ATA_STATUS_ERR);
        if (val != ATA_STATUS_DRQ)
            return -EIO;

        buffer += ATASIM_BLOCKSZ;
    }

    val = atasim_inb(atasim, ATA_REG_STATUS);
    val &= ~(ATA_STATUS_BSY | ATA_STATUS_DF | ATA_STATUS_DRQ | ATA_STATUS_ERR);
    if (iswrite)
        val &= ~ATA_STATUS_DF;

    return val ? -EIO : -ENOERR;
}

static state atasim_enqueue(struct block_device *bdev, struct block_request *breq)
{
    struct atasim_device *atasim = block_to_atasim(bdev);
    struct atasim_cmd atacmd;
    sector_t lba = breq->sector;
    state retval = -ENOERR;
    bool needext;

    if (breq->length >= (1<<8) || lba + breq->length >= (1<<8)) {
        atacmd.sector_count2 = breq->length >> 8;
        atacmd.lba_low2 = lba >> 24;
        atacmd.lba_mid2 = lba >> 32;
        atacmd.lba_high2 = lba >> 40;
        lba &= 0xffffff;
        needext = true;
    }

    if (breq->type == REQ_READ) {
        atacmd.command = needext ?
            ATA_CMD_READ_SECTORS_EXT : ATA_CMD_READ_SECTORS_WITH_RETRY;
    } else if (breq->type == REQ_WRITE) {
        atacmd.command = needext ?
            ATA_CMD_WRITE_SECTORS_EXT : ATA_CMD_WRITE_SECTORS_WITH_RETRY;
    } else {
        pr_warn("request not supported\n");
        return -EINVAL;
    }

    atacmd.sector_count = breq->length;
    atacmd.lba_low = lba;
    atacmd.lba_mid = lba >> 8;
    atacmd.lba_high = lba >> 16;
    atacmd.device = ((lba >> 24) & 0xf) | ATA_DEVSEL_LBA;

    spin_lock_irq(&atasim->host->lock);

    /* Disable interrupt */
    atasim_outb(atasim, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN);

    if (atasim_cmd(atasim, &atacmd)) {
        retval = -EIO;
        goto exit;
    }

    atasim_transfer(atasim, breq->buffer, breq->length, breq->type == REQ_WRITE);

exit:
    /* Enable interrupt */
    atasim_outb(atasim, ATA_REG_DEVCTL, ATA_DEVCTL_HD15);
    spin_unlock_irq(&atasim->host->lock);
    return retval;
}

static struct block_ops atasim_ops = {
    .enqueue = atasim_enqueue,
};

static bool atasim_port_detect(struct atasim_device *atasim)
{
    /* Wait for not-bsy */
    if (!atasim_wait_busy(atasim))
        return false;

    atasmi_devsel(atasim, 0);
    if (!atasim_wait_busy(atasim))
        return false;

    /* Check if ioport registers look valid */
    atasim_outb(atasim, ATA_REG_NSECT, 0x55);
    atasim_outb(atasim, ATA_REG_LBAL, 0xaa);

    if ((atasim_inb(atasim, ATA_REG_NSECT) != 0x55)||
        (atasim_inb(atasim, ATA_REG_LBAL) != 0xaa))
        return false;

    /* reset the channel */
    atasim_reset(atasim);

    pr_info("detected port%d on %x\n", atasim->port, atasim->host->base);
    return true;
}

static state atasim_ports_setup(struct device *dev, struct atasim_host *host)
{
    struct atasim_device *atasim;

    for (int count = 0; count < ATASIM_PORTS; ++count) {
        atasim = dev_kzalloc(dev, sizeof(*atasim), GFP_KERNEL);
        if (!atasim)
            return -ENOMEM;

        atasim->host = host;
        atasim->port = count;

        if (!atasim_port_detect(atasim)) {
            dev_kfree(dev, atasim);
            continue;
        }

        atasim->block.ops = &atasim_ops;
        block_device_register(&atasim->block);
    }

    return -ENOERR;
}

static state atasim_probe(struct pci_device *pdev, int id)
{
    struct atasim_host *host;
    resource_size_t size;
    uint32_t val;
    state retval;

    val = pdev->class >> 8;

    if (val == PCI_CLASS_STORAGE_IDE) {
        uint8_t prog = pci_config_readb(pdev, PCI_CLASS_PROG);

        if (!(prog & 0x1)) {
			pdev->resource[0].start = 0x1F0;
			pdev->resource[0].size = 0x08;
            pdev->resource[0].type = RESOURCE_PMIO;

			pdev->resource[1].start = 0x3F6;
			pdev->resource[1].size = 0x01;
            pdev->resource[1].type = RESOURCE_PMIO;
        }

        if (!(prog & 0x4)) {
			pdev->resource[2].start = 0x170;
			pdev->resource[2].size = 0x08;
            pdev->resource[2].type = RESOURCE_PMIO;

			pdev->resource[3].start = 0x376;
			pdev->resource[3].size = 0x01;
            pdev->resource[3].type = RESOURCE_PMIO;
        }
    }

    for (int count = 0; count < ATASIM_NR; ++count) {
        int bar = count * 2;

        val = pci_resource_type(pdev, bar);
        size = pci_resource_size(pdev, bar);
        if (val != RESOURCE_PMIO || size != 8)
            continue;

        host = dev_kzalloc(&pdev->dev, sizeof(*host), GFP_KERNEL);
        if (!host)
            return -ENOERR;

        host->base = pci_resource_start(pdev, bar);
        retval = atasim_ports_setup(&pdev->dev, host);
        if (retval)
            return retval;
    }

    return -ENOERR;
}

static struct pci_device_id atasim_id_table[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_STORAGE_IDE << 8, ~0xff) },
    { PCI_DEVICE_CLASS(PCI_CLASS_STORAGE_SATA << 8, ~0xff) },
};

static struct pci_driver atasim_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = atasim_id_table,
    .probe = atasim_probe,
};

static state ata_simple_init(void)
{
    return pci_driver_register(&atasim_driver);
}
driver_initcall_sync(ata_simple_init);
