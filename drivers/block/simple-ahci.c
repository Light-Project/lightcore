/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ahci-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <proc.h>
#include <delay.h>
#include <ioops.h>
#include <driver/block.h>
#include <driver/pci.h>
#include <driver/ata.h>
#include <driver/ata/atareg.h>
#include <driver/ata/ahci.h>
#include <printk.h>

#define AHCISIM_PORTS           32
#define AHCISIM_LINK_TIMEOUT    100
#define AHCISIM_RST_TIMEOUT     5000
#define AHCISIM_REQUEST_TIMEOUT 320000

struct ahcisim_host {
    struct spinlock lock;
    struct list_head ports;
    void *base;
};

struct ahcisim_port {
    struct block_device block;
    struct ahcisim_host *host;
    unsigned char port;
    bool atapi;

    struct spinlock lock;
    struct list_head list;

    struct ahci_cmd_table *cmdt;
    struct ahci_fis_table *fist;
    struct ahci_cmd_list *cmdl;
};

static __always_inline uint32_t
ahcisim_host_read(struct ahcisim_host *host, unsigned int reg)
{
    return readl(host->base + reg);
}

static __always_inline void
ahcisim_host_write(struct ahcisim_host *host, unsigned int reg, uint32_t val)
{
    writel(host->base + reg, val);
}

static __always_inline unsigned int
ahcisim_port_to_host(struct ahcisim_port *port, unsigned int reg)
{
    unsigned int port_base = AHCI_HPORTS_BASE;
    port_base += port->port * AHCI_PORT_SIZE;
    return port_base + reg;
}

static __always_inline uint32_t
ahcisim_port_read(struct ahcisim_port *port, unsigned int reg)
{
    unsigned int host_reg = ahcisim_port_to_host(port, reg);
    struct ahcisim_host *host = port->host;
    return ahcisim_host_read(host, host_reg);
}

static __always_inline void
ahcisim_port_write(struct ahcisim_port *port, unsigned int reg, uint32_t val)
{
    unsigned int host_reg = ahcisim_port_to_host(port, reg);
    struct ahcisim_host *host = port->host;
    ahcisim_host_write(host, host_reg, val);
}

static state ahcisim_port_reset(struct ahcisim_port *port)
{
    unsigned int timeout = AHCISIM_RST_TIMEOUT;
    uint32_t val;

    while (--timeout) {
        val = ahcisim_port_read(port, AHCI_CMD);
        if (!(val & (AHCI_CMD_FIS_RX | AHCI_CMD_START |
                     AHCI_CMD_FIS_ON | AHCI_CMD_LIST_ON)))
            break;
        val &= ~(AHCI_CMD_FIS_RX | AHCI_CMD_START);
        ahcisim_port_write(port, AHCI_CMD, val);
        udelay(100);
    } if (!timeout) {
        dev_err(port->block.dev, "reset timeout\n");
        return -EBUSY;
    }

    /* we don't use interrupts */
    ahcisim_port_write(port, AHCI_IRQ_MASK, 0);
    val = ahcisim_port_read(port, AHCI_IRQ_STAT);
    ahcisim_port_write(port, AHCI_IRQ_STAT, val);

    return -ENOERR;
}

static void ahcisim_cfis_cmd(struct ahci_cfis *cfis, uint8_t command)
{
    memset(cfis, 0, sizeof(*cfis));
    cfis->command = command;
}

static state ahcisim_command(struct ahcisim_port *port, void *buff,
                             unsigned int count, bool is_write)
{
    unsigned int timeout = AHCISIM_REQUEST_TIMEOUT;
    uint32_t val, error = 0, status = 0;
    irqflags_t irqflags;
    state ret = -ENOERR;

    spin_lock_irqsave(&port->lock, &irqflags);

    port->cmdt->cfis.reg      = 0x27;
    port->cmdt->cfis.pmp_type = 1 << 7;
    port->cmdt->prdt[0].base  = (uint64_t)va_to_pa(buff) & UINT32_MAX;
    port->cmdt->prdt[0].baseu = (uint64_t)va_to_pa(buff) >> 32;
    port->cmdt->prdt[0].flags = count - 1;

    val = (1 << 16) | (5 << 0);
    port->cmdl[0].flags = val;
    port->cmdl[0].bytes = 0;
    port->cmdl[0].basel = (uint64_t)va_to_pa(port->cmdt) & UINT32_MAX;
    port->cmdl[0].baseh = (uint64_t)va_to_pa(port->cmdt) >> 32;

    /* clear all interrupt flags */
    val = ahcisim_port_read(port, AHCI_IRQ_STAT);
    ahcisim_port_write(port, AHCI_IRQ_STAT, val);

    /* start command transfer */
    ahcisim_port_write(port, AHCI_CMD_ISSUE, 1);

    for (;;) {
        while (--timeout) {
            val = ahcisim_port_read(port, AHCI_IRQ_STAT);
            if (val) {
                ahcisim_port_write(port, AHCI_IRQ_STAT, val);
                if (val & AHCI_IRQ_D2H_REG_FIS) {
                    status = port->fist->rfis[2];
                    error = port->fist->rfis[3];
                    break;
                } else if (val & AHCI_IRQ_PIOS_FIS) {
                    status = port->fist->psfis[2];
                    error = port->fist->psfis[3];
                    break;
                }
            }
            udelay(100);
        }

        if (!timeout) {
            dev_err(port->block.dev, "command transfer timeout\n");
            goto exit;
        }

        if (!(status & ATA_STATUS_BSY))
            break;
    }

    if ((status & (ATA_STATUS_DF | ATA_STATUS_ERR)) || !(status & ATA_STATUS_DRDY)) {
        dev_debug(port->block.dev, "command failed with: "
            "stat (0x%08x) error (0x%08x)\n", status, error);
        ret = -EIO;
    }

exit:
    spin_unlock_irqrestore(&port->lock, &irqflags);
    return ret;
}

static struct block_ops ahcisim_ops = {
};

static bool ahcisim_port_setup(struct ahcisim_port *port)
{
    unsigned int timeout;
    uint32_t cmd, val;
    uint8_t buff[256];
    state ret;

    /* enable fis receive */
    cmd = ahcisim_port_read(port, AHCI_CMD);
    cmd |= AHCI_CMD_FIS_RX;
    ahcisim_port_write(port, AHCI_CMD, cmd);

    /* spin up device */
    cmd |= AHCI_CMD_SPIN_UP;
    ahcisim_port_write(port, AHCI_CMD, cmd);

    timeout = AHCISIM_LINK_TIMEOUT;
    while (--timeout) {
        val = ahcisim_port_read(port, AHCI_SCR_STAT);
        val &= AHCI_SCR_STAT_DET_MASK;
        if (val != AHCI_SCR_STAT_DET_DPDC) {
            udelay(100);
            continue;
        }
        dev_debug(port->block.dev, "port%d link up\n", port->port);
        break;
    } if (!timeout) {
        dev_debug(port->block.dev, "port%d link down\n", port->port);
        return false;
    }

    /* clear all error */
    val = ahcisim_port_read(port, AHCI_SCR_STAT);
    ahcisim_port_write(port, AHCI_SCR_STAT, val);

    /* wait device becoming ready */
    timeout = AHCISIM_REQUEST_TIMEOUT;
    while (--timeout) {
        val = ahcisim_port_read(port, AHCI_TFDATA);
        if (!(val & (ATA_STATUS_BSY | ATA_STATUS_DRQ)))
            break;
        udelay(100);
    } if (!timeout) {
        dev_debug(port->block.dev, "port%d not ready\n", port->port);
        return false;
    }

    /* start device */
    cmd |= AHCI_CMD_START;
    ahcisim_port_write(port, AHCI_CMD, cmd);

    ahcisim_cfis_cmd(&port->cmdt->cfis, ATA_CMD_IDENTIFY_PACKET);
    ret = ahcisim_command(port, buff, sizeof(buff), false);
    if (!ret) {
        port->atapi = true;
    } else {
        ahcisim_cfis_cmd(&port->cmdt->cfis, ATA_CMD_IDENTIFY_DEVICE);
        ret = ahcisim_command(port, NULL, 0, false);
        if (ret)
            return false;
    }

    return true;
}

static struct ahcisim_port *ahcisim_port_alloc(struct pci_device *pdev)
{
    struct ahcisim_port *port;
    size_t size;

    port = pci_kzalloc(pdev, sizeof(*port), GFP_KERNEL);
    if (!port)
        return NULL;

    size = AHCI_CMD_LIST_SIZE;
    port->cmdl = pci_kzalloc_align(pdev, size, GFP_KERNEL, size);
    if (!port->cmdl)
        goto error_cmdl;

    size = sizeof(*port->fist);
    port->fist = pci_kzalloc_align(pdev, size, GFP_KERNEL, size);
    if (!port->fist)
        goto error_fist;

    size = sizeof(*port->cmdt) + (sizeof(struct ahci_prdt) * 8);
    port->cmdt = pci_kzalloc_align(pdev, size, GFP_KERNEL, size);
    if (!port->cmdt)
        goto error_cmdt;

    port->block.dev = &pdev->dev;
    port->block.ops = &ahcisim_ops;
    return port;

error_cmdt:
    pci_kfree(pdev, port->fist);
error_fist:
    pci_kfree(pdev, port->cmdl);
error_cmdl:
    pci_kfree(pdev, port);
    return NULL;
}

static void ahcisim_ports_free(struct ahcisim_port *port)
{
    dev_kfree(port->block.dev, port->cmdl);
    dev_kfree(port->block.dev, port->cmdt);
    dev_kfree(port->block.dev, port->fist);
    dev_kfree(port->block.dev, port);
}

static void ahcisim_port_relocate(struct ahcisim_port *port)
{
    ahcisim_port_write(port, AHCI_LST_ADDR, (uint64_t)va_to_pa(port->cmdl) & UINT32_MAX);
    ahcisim_port_write(port, AHCI_LST_ADDR_HI, (uint64_t)va_to_pa(port->cmdl) >> 32);
    ahcisim_port_write(port, AHCI_FIS_ADDR, (uint64_t)va_to_pa(port->fist) & UINT32_MAX);
    ahcisim_port_write(port, AHCI_FIS_ADDR_HI, (uint64_t)va_to_pa(port->fist) >> 32);
}

static struct ahcisim_port *ahcisim_port_create(struct pci_device *pdev, struct ahcisim_port *src)
{
    struct ahcisim_host *host = src->host;
    struct ahcisim_port *port;
    uint32_t val;

    port = ahcisim_port_alloc(pdev);
    if (!port)
        return NULL;

    port->host = src->host;
    port->port = src->port;
    port->atapi = src->atapi;

    ahcisim_port_relocate(port);
    list_add(&host->ports, &port->list);

    val = ahcisim_port_read(port, AHCI_CMD);
    val |= AHCI_CMD_FIS_RX | AHCI_CMD_START;
    ahcisim_port_write(port, AHCI_CMD, val);

    return port;
}

static state ahcisim_ports_scan(struct pci_device *pdev, struct ahcisim_port *scan, unsigned int pnr)
{
    struct ahcisim_host *host = pci_get_devdata(pdev);
    struct ahcisim_port *port;

    scan->host = host;
    scan->port = pnr;
    scan->block.dev = &pdev->dev;
    scan->block.ops = &ahcisim_ops;

    ahcisim_port_relocate(scan);
    ahcisim_port_reset(scan);

    if (!ahcisim_port_setup(scan))
        return -ENOERR;

    port = ahcisim_port_create(pdev, scan);
    if (!port)
        return -ENOMEM;

    return block_device_register(&port->block);
}

static void ahcisim_host_setup(struct pci_device *pdev)
{
    struct ahcisim_host *host = pci_get_devdata(pdev);
    uint32_t val;

    pci_device_enable(pdev);
    pci_master_enable(pdev);

    val = ahcisim_host_read(host, AHCI_HCTL);
    val |= AHCI_HCTL_AHCI_EN;
    ahcisim_host_write(host, AHCI_HCTL, val);

    val = ahcisim_host_read(host, AHCI_HVERSION);
    pci_debug(pdev, "version %u\n", val);
}

static state ahcisim_probe(struct pci_device *pdev, const void *pdata)
{
    struct ahcisim_host *host;
    struct ahcisim_port *scan;
    unsigned int count;
    state ret;

    host = pci_kzalloc(pdev, sizeof(*host), GFP_KERNEL);
    if (!host)
        return -ENOMEM;

    host->base = pci_resource_ioremap(pdev, 5, 0);
    if (!host->base)
        return -ENOMEM;

    list_head_init(&host->ports);
    pci_set_devdata(pdev, host);
    ahcisim_host_setup(pdev);

    scan = ahcisim_port_alloc(pdev);
    if (!scan)
        return -ENOMEM;

    for (count = 0; count < AHCISIM_PORTS; ++count) {
        ret = ahcisim_ports_scan(pdev, scan, count);
        if (ret < 0)
            return ret;
    }

    ahcisim_ports_free(scan);
    return -ENOERR;
}

static const struct pci_device_id ahcisim_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_STORAGE_SATA_AHCI, ~0) },
    { }, /* NULL */
};

static struct pci_driver ahcisim_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = ahcisim_ids,
    .probe = ahcisim_probe,
};

/* priority to use other drivers (sync init) */
static state ahcisim_init(void)
{
    return pci_driver_register(&ahcisim_driver);
}
driver_initcall_sync(ahcisim_init);
