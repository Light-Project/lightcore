/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i386-pci"

#include <initcall.h>
#include <ioops.h>
#include <crash.h>
#include <driver/platform.h>
#include <driver/pci.h>

#define I386_PCI_CMD  0x0cf8
#define I386_PCI_DATA 0x0cfc

#define I386_PCI_CONFIG(bus, devfn, reg) (  \
    BIT(31) | (bus << 16) | (devfn << 8) |  \
    ((reg & 0xF00) << 16) | (reg & 0xfc)    \
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
    outl(I386_PCI_CMD, I386_PCI_CONFIG(bus, devfn, reg));

    switch (size) {
        case 1: default:
            *val = inb(I386_PCI_DATA + offset);
            break;

        case 2:
            WARN_ON_ONCE(!align_check(offset, 2));
            *val = inw(I386_PCI_DATA + offset);
            break;

        case 4:
            WARN_ON_ONCE(!align_check(offset, 4));
            *val = inl(I386_PCI_DATA + offset);
            break;
    }

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

    spin_lock_irqsave(&i386_pci_lock, &irqflags);

    /* Use North Bridge Port */
    outl(I386_PCI_CMD, I386_PCI_CONFIG(bus, devfn, reg));

    switch (size) {
        case 1: default:
            outb(I386_PCI_DATA + offset, val);
            break;

        case 2:
            WARN_ON_ONCE(!align_check(offset, 2));
            outw(I386_PCI_DATA + offset, val);
            break;

        case 4:
            WARN_ON_ONCE(!align_check(offset, 4));
            outl(I386_PCI_DATA + offset, val);
            break;
    }

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
