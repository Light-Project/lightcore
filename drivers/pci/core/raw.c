/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

state __weak pci_raw_config_read(unsigned int domain, unsigned int bus, unsigned int devfn,
                                 unsigned int reg, int size, uint32_t *val)
{
	struct pci_bus *pbus = pci_find_bus(domain, bus);

    if (!pbus)
        return -ENODEV;
    return pbus->ops->read(pbus, devfn, reg, size, val);
}

state __weak pci_raw_config_write(unsigned int domain, unsigned int bus, unsigned int devfn,
                                  unsigned int reg, int size, uint32_t val)
{
	struct pci_bus *pbus = pci_find_bus(domain, bus);

    if (!pbus)
        return -ENODEV;
    return pbus->ops->write(pbus, devfn, reg, size, val);
}

EXPORT_SYMBOL(pci_raw_config_read);
EXPORT_SYMBOL(pci_raw_config_write);
