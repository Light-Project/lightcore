/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ata-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <driver/block.h>
#include <init/initcall.h>
#include <driver/pci.h>
#include <driver/ata.h>
#include <printk.h>

#include <asm/io.h>

struct atasim_device {
    struct spinlock lock;
    struct device *device;
    struct block_device block;
    resource_size_t port;
};

#define block_to_atasim(bdev) \
    container_of(bdev, struct atasim_device, block);

static __always_inline uint8_t 
atasim_inb(struct atasim_device *atasim, int reg)
{
    return inb(atasim->port + reg);
}

static __always_inline uint16_t 
atasim_inw(struct atasim_device *atasim, int reg)
{
    return inw(atasim->port + reg);
}

static __always_inline void 
atasim_outb(struct atasim_device *atasim, int reg, uint8_t val)
{
    outb(atasim->port + reg, val);
}

static state atasim_read(struct atasim_device *atasim, struct block_request *breq)
{
    uint16_t *buff = breq->buffer;
    int len;

    atasim_outb(atasim, ATA_REG_DEVSEL, 0xE0);
    atasim_outb(atasim, ATA_REG_FEATURE, 0x00);
    atasim_outb(atasim, ATA_REG_NSECT, breq->sector_nr);
    atasim_outb(atasim, ATA_REG_LBAL, breq->sector);
    atasim_outb(atasim, ATA_REG_LBAM, breq->sector >> 8);
    atasim_outb(atasim, ATA_REG_LBAH, breq->sector >> 16);
    atasim_outb(atasim, ATA_REG_CMD, ATA_COMMAND_READ_SECTORS_WITH_RETRY);

    len = breq->sector_nr * 256;
    while (len--) {
        while(!(atasim_inb(atasim, ATA_REG_STATUS) &
                ATA_STATUS_DRQ));
        *buff++ = atasim_inw(atasim, ATA_REG_DATA);
    }

    return -ENOERR;
}

static state atasim_write(struct atasim_device *atasim, struct block_request *breq)
{
    atasim_outb(atasim, ATA_REG_DEVSEL, 0xE0);
    atasim_outb(atasim, ATA_REG_FEATURE, 0x00);
    atasim_outb(atasim, ATA_REG_NSECT, breq->sector_nr);
    atasim_outb(atasim, ATA_REG_LBAL, breq->sector);
    atasim_outb(atasim, ATA_REG_LBAM, breq->sector >> 8);
    atasim_outb(atasim, ATA_REG_LBAH, breq->sector >> 16);
    atasim_outb(atasim, ATA_REG_CMD, ATA_COMMAND_WRITE_SECTORS_WITH_RETRY);

    return -ENOERR;
}

static state atasim_enqueue(struct block_device *bdev, struct block_request *breq)
{
    struct atasim_device *atasim = block_to_atasim(bdev);
    state retval;

    spin_lock_irq(&atasim->lock);

    switch (breq->type) {
        case BLOCK_REQ_READ:
            retval = atasim_read(atasim, breq);
            break;
        case BLOCK_REQ_WRITE:
            retval = atasim_write(atasim, breq);
            break;
        default:
            pr_warn("request not supported\n");
            retval = -EINVAL;
    }

    spin_unlock_irq(&atasim->lock);
    return retval;
}

static struct block_ops atasim_ops = {
    .enqueue = atasim_enqueue,
};

static state atasim_probe(struct pci_device *pdev, int id)
{
    struct atasim_device *atasim;
    // resource_size_t port;
    // int count;
    

    // for (count = 0; count < PCI_STD_NUM_BARS; ++count) {
    //     if (pci_resource_type(pdev, count) != RESOURCE_PMIO ||
    //         pci_resource_size(pdev, count) != 8)
    //         continue;
    //     port = pci_resource_start(pdev, count);
    //     break;
    // } if (count == PCI_ROM_RESOURCE) {
    //     pr_warn("no i/o available\n");
    //     return -ENODEV;
    // }

    atasim = kzalloc(sizeof(*atasim), GFP_KERNEL);
    if (!atasim)
        return -ENOMEM;

    atasim->device = &pdev->dev;
    atasim->port = 0x1f0;
    atasim->block.ops = &atasim_ops;
    block_device_register(&atasim->block);

    return -ENOERR;
}

static state atasim_remove(struct pci_device *pdev)
{
    
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
    .remove = atasim_remove,
};

static state ata_simple_init(void)
{
    return pci_driver_register(&atasim_driver);
}
driver_initcall_sync(ata_simple_init);
