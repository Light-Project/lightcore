/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i386-pci"

#include <initcall.h>
#include <ioops.h>
#include <driver/platform.h>
#include <driver/pci.h>

#define I386_PCI_CONFIG(bus, devfn, reg) ( \
    BIT(31) | (bus << 16) | (devfn << 8) | \
    ((reg & 0xF00) << 16) | (reg & 0xfc) \
)

static SPIN_LOCK(i386_pci_lock);

state pci_raw_config_read(unsigned int domain, unsigned int bus, unsigned int devfn,
                          unsigned int reg, int size, uint32_t *val)
{
    unsigned int offset = reg & 0x03;
    irqflags_t irqflags;

    if (domain || (bus > 255) || (devfn > 255) || (reg > 4095)) {
        *val = UINT32_MAX;
        return -EINVAL;
    }

    spin_lock_irqsave(&i386_pci_lock, &irqflags);

    /* Use North Bridge Port */
    outl(0x0cf8, I386_PCI_CONFIG(bus, devfn, reg));

    if (size == 1)
        *val = inb(0x0cfc + offset);
    else if (size == 2)
        *val = inw(0x0cfc + offset);
    else
        *val = inl(0x0cfc + offset);

    spin_unlock_irqrestore(&i386_pci_lock, &irqflags);

    return -ENOERR;
}

state pci_raw_config_write(unsigned int domain, unsigned int bus, unsigned int devfn,
                           unsigned int reg, int size, uint32_t val)
{
    unsigned int offset = reg & 0x03;
    irqflags_t irqflags;

    if (domain || (bus > 255) || (devfn > 255) || (reg > 4095))
        return -EINVAL;

    /* Use North Bridge Port */
    outl(0x0cf8, I386_PCI_CONFIG(bus, devfn, reg));

    spin_lock_irqsave(&i386_pci_lock, &irqflags);

    if (size == 1)
        outb(0x0cfc, val + offset);
    else if (size == 2)
        outw(0x0cfc, val + offset);
    else
        outl(0x0cfc, val + offset);

    spin_unlock_irqrestore(&i386_pci_lock, &irqflags);

    return -ENOERR;
}

static state pci_config_read(struct pci_bus *bus, unsigned int devfn, unsigned int reg, int size, uint32_t *val)
{
    return pci_raw_config_read(0, bus->bus_nr, devfn, reg, size, val);
}

static state pci_config_write(struct pci_bus *bus, unsigned int devfn, unsigned int reg, int size, uint32_t val)
{
    return pci_raw_config_write(0, bus->bus_nr, devfn, reg, size, val);
}

static struct pci_ops i386pci_ops = {
    .read = pci_config_read,
    .write = pci_config_write,
};

static state i386pci_probe(struct platform_device *pdev, const void *pdata)
{
    struct pci_host *host;

    host = platform_kzalloc(pdev, sizeof(*host), GFP_KERNEL);
    if(!host)
        return -ENOMEM;

    host->ops = &i386pci_ops;
    return pci_host_register(host);
}

static struct dt_device_id i386pci_id[] = {
    { .compatible = "intel,i386-pci" },
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
