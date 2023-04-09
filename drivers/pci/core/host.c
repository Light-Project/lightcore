/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

struct pci_bus *pci_bus_find_ancestor(const struct pci_bus *bus)
{
    while (bus->parent)
        bus = bus->parent;

    return (struct pci_bus *)bus;
}
EXPORT_SYMBOL(pci_bus_find_ancestor);

struct pci_host *pci_bus_find_host(const struct pci_bus *bus)
{
    struct pci_bus *ancestor = pci_bus_find_ancestor(bus);
    return ancestor->host;
}
EXPORT_SYMBOL(pci_bus_find_host);
