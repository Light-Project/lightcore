/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i386pci"

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/pci.h>

#include <asm/io.h>

static state pci_config_read(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t *val)
{
    uint32_t addr = (1 << 31) | (bus->bus_nr << 16) | (PCI_SLOT(devfn) << 11) |
                    (PCI_FUNC(devfn)<<8) | reg;

    /* Use North Bridge Port */
    outl(0x0cf8, addr);

    if (size == 1)
        *val = inb(0x0cfc);
    else if (size == 2)
        *val = inw(0x0cfc);
    else
        *val = inl(0x0cfc);

    return -ENOERR;
}

static state pci_config_write(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t val)
{
    uint32_t addr = (1 << 31) | (bus->bus_nr << 16) | (PCI_SLOT(devfn) << 11) |
                    (PCI_FUNC(devfn)<<8) | reg;

    /* Use North Bridge Port */
    outl(0x0cf8, addr);

    if (size == 1)
        outb(0x0cfc, val);
    else if (size == 2)
        outw(0x0cfc, val);
    else
        outl(0x0cfc, val);

    return -ENOERR;
}

static struct pci_ops i386pci_ops = {
    .read = pci_config_read,
    .write = pci_config_write,
};

static state i386pci_probe(struct platform_device *pdev)
{
    struct pci_host *host;

    host = kmalloc(sizeof(*host), GFP_KERNEL);
    if(!host)
        return -ENOMEM;

    host->ops = &i386pci_ops;

    return pci_host_register(host);
}

static struct dt_device_id i386pci_id[] = {
    { .compatible = "i386-pci" },
    { }, /* NULL */
};

static struct platform_driver i386pci_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i386pci_id,
    .probe = i386pci_probe,
};

static state i386pci_init(void)
{
    return platform_driver_register(&i386pci_driver);
}
driver_initcall(i386pci_init);