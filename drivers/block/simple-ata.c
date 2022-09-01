/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ata-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <printk.h>
#include <delay.h>
#include <proc.h>
#include <ioops.h>
#include <driver/block.h>
#include <driver/pci.h>
#include <driver/ata.h>

#define ATASIM_NR       2
#define ATASIM_PORTS    2
#define ATASIM_BLOCKSZ  512
#define ATASIM_TIMEOUT  5000

struct atasim_host {
    spinlock_t lock;
    resource_size_t cmd, ctl;
    struct pci_device *pdev;
    struct list_head ports;
    unsigned char host;
};

struct atasim_port {
    struct block_device block;
    struct atasim_host *host;
    struct list_head list;
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
    container_of(bdev, struct atasim_port, block)

static __always_inline uint8_t
atasim_cmd_in(struct atasim_host *host, int reg)
{
    return inb(host->cmd + reg);
}

static __always_inline void
atasim_cmd_out(struct atasim_host *host, int reg, uint8_t val)
{
    outb(host->cmd + reg, val);
}

static __always_inline uint8_t
atasim_ctl_in(struct atasim_host *host, int reg)
{
    return inb(host->ctl + reg);
}

static __always_inline void
atasim_ctl_out(struct atasim_host *host, int reg, uint8_t val)
{
    outb(host->ctl + reg, val);
}

static bool atasim_wait_poweron(struct atasim_host *host)
{
    uint8_t status, val = 0;
    unsigned int timeout = ATASIM_TIMEOUT;

    while (--timeout) {
        status = atasim_cmd_in(host, ATA_REG_STATUS);
        if (!(status & ATA_STATUS_BSY))
            break;

        val |= status;
        if (val == 0xff)
            return false;

        udelay(100);
    }

    return !!timeout;
}

static bool atasim_wait(struct atasim_host *host, uint8_t mask,
                        int8_t pass, unsigned int timeout)
{
    uint8_t val;

    while (--timeout) {
        val = atasim_cmd_in(host, ATA_REG_STATUS);
        if ((val & mask) == pass)
            break;
        udelay(100);
    }

    return !!timeout;
}

static inline bool atasim_wait_ready(struct atasim_host *host)
{
    return atasim_wait(host, ATA_STATUS_DRDY, ATA_STATUS_DRDY, ATASIM_TIMEOUT);
}

static inline bool atasim_wait_busy(struct atasim_host *host)
{
    return atasim_wait(host, ATA_STATUS_BSY, 0, ATASIM_TIMEOUT);
}

static inline bool atasim_pause_wait_busy(struct atasim_host *host)
{
    /* wait one pio transfer cycle */
    atasim_ctl_in(host, ATA_REG_ASTAT);
    return atasim_wait_busy(host);
}

static inline bool atasim_delay_wait_busy(struct atasim_host *host)
{
    /* wait 400ns first */
    ndelay(400);
    return atasim_wait_busy(host);
}

static inline state atasim_wait_data(struct atasim_host *host)
{
    uint8_t val;

    if (!atasim_delay_wait_busy(host))
        return -EBUSY;

    val = atasim_cmd_in(host, ATA_REG_STATUS);
    if (val & ATA_STATUS_ERR)
        return -EFAULT;

    if (!(val & ATA_STATUS_DRQ))
        return -ENODATA;

    return -ENOERR;
}

static state atasim_reset(struct atasim_host *host)
{
    /* pulse reset device */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN | ATA_DEVCTL_SRST);
    udelay(5);
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN);
    mdelay(2);

    /* setup device on reset */
    atasim_cmd_out(host, ATA_REG_DEVSEL, ATA_DEVSEL_IBM);

    /* wait for device to become not busy */
    if (!atasim_wait_ready(host)) {
        pci_debug(host->pdev, "host%d reset timeout\n", host->host);
        return -ENODEV;
    }

    /* enable interrupts */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15);
    return -ENOERR;
}

static uint8_t atasmi_devsel(struct atasim_port *port, uint8_t device)
{
    struct atasim_host *host = port->host;
    device |= ATA_DEVSEL_IBM;

    if (port->port)
        device |= ATA_DEVSEL_DEV;

    atasim_cmd_out(host, ATA_REG_DEVSEL, device);
    return device;
}

static state atasim_cmd(struct atasim_port *port, struct atasim_cmd *cmd)
{
    struct atasim_host *host = port->host;

    if (!atasim_wait_busy(host))
        return -EIO;

    /* select device */
    atasmi_devsel(port, cmd->device);
    if (!atasim_wait_busy(host))
        return -EIO;

    /* check for ATA_CMD_(READ|WRITE)_(SECTORS|DMA)_EXT commands. */
    if ((cmd->command & ~0x11) == ATA_CMD_READ_SECTORS_EXT) {
        atasim_cmd_out(host, ATA_REG_FEATURE, cmd->feature2);
        atasim_cmd_out(host, ATA_REG_NSECT, cmd->sector_count2);
        atasim_cmd_out(host, ATA_REG_LBAL, cmd->lba_low2);
        atasim_cmd_out(host, ATA_REG_LBAM, cmd->lba_mid2);
        atasim_cmd_out(host, ATA_REG_LBAH, cmd->lba_high2);
    }

    atasim_cmd_out(host, ATA_REG_FEATURE, cmd->feature);
    atasim_cmd_out(host, ATA_REG_NSECT, cmd->sector_count);
    atasim_cmd_out(host, ATA_REG_LBAL, cmd->lba_low);
    atasim_cmd_out(host, ATA_REG_LBAM, cmd->lba_mid);
    atasim_cmd_out(host, ATA_REG_LBAH, cmd->lba_high);
    atasim_cmd_out(host, ATA_REG_CMD, cmd->command);

    return -ENOERR;
}

static state atasim_transfer(struct atasim_host *host, void *buffer,
                             unsigned int count, bool iswrite)
{
    uint8_t val;

    for (; count; --count) {
        if (iswrite) { /* write data to controller */
#ifdef CONFIG_BLK_ATASIM32
            outsl(host->cmd, buffer, ATASIM_BLOCKSZ / 4);
#else
            outsw(host->cmd, buffer, ATASIM_BLOCKSZ / 2);
#endif
        } else { /* read data to controller */
#ifdef CONFIG_BLK_ATASIM32
            insl(host->cmd, buffer, ATASIM_BLOCKSZ / 4);
#else
            insw(host->cmd, buffer, ATASIM_BLOCKSZ / 2);
#endif
        }

        if (!atasim_pause_wait_busy(host))
            return -EIO;

        val = atasim_cmd_in(host, ATA_REG_STATUS);
        val &= (ATA_STATUS_BSY | ATA_STATUS_DRQ | ATA_STATUS_ERR);
        if (val != ATA_STATUS_DRQ)
            return -EIO;

        buffer += ATASIM_BLOCKSZ;
    }

    val = atasim_cmd_in(host, ATA_REG_STATUS);
    val &= ~(ATA_STATUS_BSY | ATA_STATUS_DF | ATA_STATUS_DRQ | ATA_STATUS_ERR);
    if (iswrite)
        val &= ~ATA_STATUS_DF;

    return val ? -EIO : -ENOERR;
}

static state atasim_identity(struct atasim_port *port, void *buffer, uint8_t cmd)
{
    struct atasim_host *host = port->host;
    struct atasim_cmd atacmd = {};
    state retval;

    memset(buffer, 0, 512);
    atacmd.command = cmd;

    /* disable interrupt */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN);

    retval = atasim_cmd(port, &atacmd);
    if (retval)
        goto exit;

    retval = atasim_wait_data(host);
    if (retval)
        goto exit;

    atasim_transfer(host, buffer, 1, false);

exit:
    /* enable interrupt */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15);
    return retval;
}

static state atasim_enqueue(struct block_device *bdev, struct block_request *breq)
{
    struct atasim_port *port = block_to_atasim(bdev);
    struct atasim_host *host = port->host;
    struct atasim_cmd atacmd = {};
    sector_t lba = breq->sector;
    irqflags_t irqflags;
    state retval = -ENOERR;
    bool needext = false;

    if (breq->length >= (1 << 8) || lba + breq->length >= (1 << 8)) {
        atacmd.sector_count2 = breq->length >> 8;
        atacmd.lba_low2 = lba >> 24;
        atacmd.lba_mid2 = lba >> 32;
        atacmd.lba_high2 = lba >> 40;
        lba &= 0xffffff;
        needext = true;
    }

    if (breq->type == BLK_REQ_READ) {
        atacmd.command = needext ?
            ATA_CMD_READ_SECTORS_EXT : ATA_CMD_READ_SECTORS_WITH_RETRY;
    } else if (breq->type == BLK_REQ_WRITE) {
        atacmd.command = needext ?
            ATA_CMD_WRITE_SECTORS_EXT : ATA_CMD_WRITE_SECTORS_WITH_RETRY;
    } else {
        pci_warn(host->pdev, "host%d:port%d request not supported\n", host->host, port->port);
        return -EINVAL;
    }

    atacmd.sector_count = breq->length;
    atacmd.lba_low = lba;
    atacmd.lba_mid = lba >> 8;
    atacmd.lba_high = lba >> 16;
    atacmd.device = ((lba >> 24) & 0xf) | ATA_DEVSEL_LBA;

    spin_lock_irqsave(&host->lock, &irqflags);

    /* disable interrupt */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15 | ATA_DEVCTL_NIEN);

    retval = atasim_cmd(port, &atacmd);
    if (retval)
        goto exit;

    retval = atasim_wait_data(host);
    if (retval)
        goto exit;

    atasim_transfer(host, breq->buffer, breq->length, breq->type == BLK_REQ_WRITE);

exit:
    /* enable interrupt */
    atasim_ctl_out(host, ATA_REG_DEVCTL, ATA_DEVCTL_HD15);
    spin_unlock_irqrestore(&host->lock, &irqflags);
    return retval;
}

static struct block_ops atasim_ops = {
    .enqueue = atasim_enqueue,
};

static bool atasim_check_atapi(struct atasim_port *port)
{
    struct ata_identify_table table;
    state retval;

    retval = atasim_identity(port, &table, ATA_CMD_IDENTIFY_PACKET);
    if (retval)
        return false;

    return true;
}

static bool atasim_check_devata(struct atasim_port *port)
{
    struct ata_identify_table table;
    state retval;

    retval = atasim_identity(port, &table, ATA_CMD_IDENTIFY_DEVICE);
    if (retval)
        return false;

    return true;
}

static bool atasim_port_setup(struct atasim_port *port)
{
    struct atasim_host *host = port->host;
    uint8_t val;

    /* wait for not-bsy */
    if (!atasim_wait_poweron(host))
        return false;

    val = atasmi_devsel(port, 0);
    ndelay(400);
    if (!atasim_wait_poweron(host))
        return false;

    /* check if ioport registers look valid */
    atasim_cmd_out(host, ATA_REG_DEVSEL, val);
    atasim_cmd_out(host, ATA_REG_NSECT, 0x55);
    atasim_cmd_out(host, ATA_REG_LBAL, 0xaa);

    if ((atasim_cmd_in(host, ATA_REG_NSECT) != 0x55) ||
        (atasim_cmd_in(host, ATA_REG_LBAL) != 0xaa)  ||
        (atasim_cmd_in(host, ATA_REG_DEVSEL) != val))
        return false;

    if (!(atasim_check_atapi(port) ||
        atasim_check_devata(port)))
        return false;

    pci_info(host->pdev, "host%d:port%d detected\n", host->host, port->port);
    return true;
}

static struct atasim_port *atasim_port_create(struct pci_device *pdev, struct atasim_port *src)
{
    struct atasim_host *host = src->host;
    struct atasim_port *port;

    port = pci_kzalloc(pdev, sizeof(*port), GFP_KERNEL);
    if (!port)
        return NULL;

    port->host = src->host;
    port->port = src->port;
    port->block.dev = src->block.dev;
    port->block.ops = src->block.ops;

    list_add(&host->ports, &port->list);
    return port;
}

static state atasim_ports_scan(struct pci_device *pdev, struct atasim_host *host, unsigned int pnr)
{
    struct atasim_port *port, scan;

    scan.host = host;
    scan.port = pnr;
    scan.block.dev = &pdev->dev;
    scan.block.ops = &atasim_ops;

    if (!atasim_port_setup(&scan))
        return -ENOERR;

    port = atasim_port_create(pdev, &scan);
    if (!port)
        return -ENOMEM;

    return block_device_register(&port->block);
}

static state atasim_probe(struct pci_device *pdev, const void *pdata)
{
    unsigned int hostc, portc, online = 0;
    struct atasim_host *host;
    resource_size_t size;
    state ret = -ENOERR;
    uint32_t val;

    /* legacy mode ATA controllers have fixed addresses */
    val = pci_config_readb(pdev, PCI_CLASS_PROG);

    if (!(val & 0x1)) {
        pdev->resource[0].start = 0x1F0;
        pdev->resource[0].size = 0x08;
        pdev->resource[0].type = RESOURCE_PMIO;

        pdev->resource[1].start = 0x3F6;
        pdev->resource[1].size = 0x04;
        pdev->resource[1].type = RESOURCE_PMIO;
    }

    if (!(val & 0x4)) {
        pdev->resource[2].start = 0x170;
        pdev->resource[2].size = 0x02;
        pdev->resource[2].type = RESOURCE_PMIO;

        pdev->resource[3].start = 0x376;
        pdev->resource[3].size = 0x02;
        pdev->resource[3].type = RESOURCE_PMIO;
    }

    for (hostc = 0; hostc < ATASIM_NR; ++hostc) {
        unsigned int bar = hostc * 2;

        val = pci_resource_type(pdev, bar);
        size = pci_resource_size(pdev, bar);
        if (val != RESOURCE_PMIO || size != 8)
            continue;

        host = pci_kzalloc(pdev, sizeof(*host), GFP_KERNEL);
        if (!host)
            return -ENOMEM;

        host->cmd = pci_resource_start(pdev, bar);
        host->ctl = pci_resource_start(pdev, bar + 1);
        host->pdev = pdev;
        host->host = hostc;
        list_head_init(&host->ports);

        ret = atasim_reset(host);
        if (ret)
            continue;

        for (portc = 0; portc < ATASIM_PORTS; ++portc) {
            ret = atasim_ports_scan(pdev, host, portc);
            if (ret)
                continue;
            online++;
        }
    }

    return online ? -ENOERR : ret;
}

static const struct pci_device_id atasim_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_STORAGE_IDE << 8, ~0xff) },
    { }, /* NULL */
};

static struct pci_driver atasim_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = atasim_ids,
    .probe = atasim_probe,
};

/* priority to use other drivers (sync init) */
static state atasim_init(void)
{
    return pci_driver_register(&atasim_driver);
}
driver_initcall_sync(atasim_init);
