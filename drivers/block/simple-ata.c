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

#include <asm/proc.h>
#include <asm/io.h>

#define ATASIM_NR    2
#define ATASIM_PORTS 2

struct atasim_device {
    struct spinlock lock;
    struct block_device block;
    resource_size_t base;
    uint8_t port;
};

#define block_to_atasim(bdev) \
    container_of(bdev, struct atasim_device, block);

static __always_inline uint8_t
atasim_inb(struct atasim_device *atasim, int reg)
{
    return inb(atasim->base + reg);
}

static __always_inline uint16_t
atasim_inw(struct atasim_device *atasim, int reg)
{
    return inw(atasim->base + reg);
}

static __always_inline void
atasim_outb(struct atasim_device *atasim, int reg, uint8_t val)
{
    outb(atasim->base + reg, val);
}

static __always_inline void
atasim_outw(struct atasim_device *atasim, int reg, uint16_t val)
{
    outw(atasim->base + reg, val);
}

static void atasmi_devsel(struct atasim_device *atasim)
{
    uint8_t val = ATA_DEVSEL_OBS;

    if (atasim->port)
        val |= ATA_DEVSEL_DEV;

    atasim_outb(atasim, ATA_REG_DEVSEL, val);
}

static state atasim_read(struct atasim_device *atasim, struct block_request *breq)
{
    uint16_t *buff = breq->buffer;
    uint32_t val;

    val = ((!!atasim->port) << 4) | ((breq->sector >> 24) & 0x0f);
    atasim_outb(atasim, ATA_REG_DEVSEL, ATA_DEVSEL_OBS | ATA_DEVSEL_LBA | val);
    atasim_outb(atasim, ATA_REG_LBAL, breq->sector);
    atasim_outb(atasim, ATA_REG_LBAM, breq->sector >> 8);
    atasim_outb(atasim, ATA_REG_LBAH, breq->sector >> 16);
    atasim_outb(atasim, ATA_REG_NSECT, breq->sector_nr);
    atasim_outb(atasim, ATA_REG_CMD, ATA_CMD_READ_SECTORS_WITH_RETRY);

    val = breq->sector_nr << 8;

    while (val--) {
        while(!(atasim_inb(atasim, ATA_REG_STATUS) & ATA_STATUS_DRQ))
            cpu_relax();
        *buff++ = atasim_inw(atasim, ATA_REG_DATA);
    }

    return -ENOERR;
}

static state atasim_write(struct atasim_device *atasim, struct block_request *breq)
{
    uint16_t *buff = breq->buffer;
    uint32_t val;

    val = 0xE0 | ((!!atasim->port) << 4) | ((breq->sector >> 24) & 0x0f);
    atasim_outb(atasim, ATA_REG_DEVSEL, val);
    atasim_outb(atasim, ATA_REG_NSECT, breq->sector_nr);
    atasim_outb(atasim, ATA_REG_LBAL, breq->sector);
    atasim_outb(atasim, ATA_REG_LBAM, breq->sector >> 8);
    atasim_outb(atasim, ATA_REG_LBAH, breq->sector >> 16);
    atasim_outb(atasim, ATA_REG_CMD, ATA_CMD_WRITE_SECTORS_WITH_RETRY);

    val = breq->sector_nr << 8;

    while (val--) {
        while(!(atasim_inb(atasim, ATA_REG_STATUS) & ATA_STATUS_DRQ))
            cpu_relax();
        atasim_outw(atasim, ATA_REG_DATA, *buff++);
    }

    return -ENOERR;
}

static state atasim_enqueue(struct block_device *bdev, struct block_request *breq)
{
    struct atasim_device *atasim = block_to_atasim(bdev);
    state retval;

    spin_lock_irq(&atasim->lock);

    switch (breq->type) {
        case REQ_READ:
            retval = atasim_read(atasim, breq);
            break;
        case REQ_WRITE:
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

static state atasim_port_setup(struct atasim_device *atasim)
{
    /* Device presence detection */
    atasmi_devsel(atasim);
    atasim_outb(atasim, ATA_REG_NSECT, 0x55);
    atasim_outb(atasim, ATA_REG_LBAL, 0xaa);

    atasim_outb(atasim, ATA_REG_NSECT, 0xaa);
    atasim_outb(atasim, ATA_REG_LBAL, 0x55);

    atasim_outb(atasim, ATA_REG_NSECT, 0x55);
    atasim_outb(atasim, ATA_REG_LBAL, 0xaa);

    if ((atasim_inb(atasim, ATA_REG_NSECT) != 0x55)||
        (atasim_inb(atasim, ATA_REG_LBAL) != 0xaa))
        return -ENODEV;

    pr_info("detected port%d on %x\n", atasim->port, atasim->base);

    atasim->block.ops = &atasim_ops;
    block_device_register(&atasim->block);
    return -ENOERR;
}

static state atasim_ports_setup(struct device *dev, resource_size_t base)
{
    struct atasim_device *atasim;
    state retval;

    for (int count = 0; count < ATASIM_PORTS; ++count) {
        atasim = dev_kzalloc(dev, sizeof(*atasim), GFP_KERNEL);
        if (!atasim)
            return -ENOMEM;

        atasim->base = base;
        atasim->port = count;

        retval = atasim_port_setup(atasim);
        if (retval)
            return retval;
    }

    return -ENOERR;
}

static state atasim_probe(struct pci_device *pdev, int id)
{
    resource_size_t base;
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
        base = pci_resource_size(pdev, bar);
        if (val != RESOURCE_PMIO || base != 8)
            continue;

        base = pci_resource_start(pdev, bar);
        retval = atasim_ports_setup(&pdev->dev, base);
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
