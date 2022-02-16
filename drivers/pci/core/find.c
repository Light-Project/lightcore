/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

struct pci_device *pci_get_device(struct pci_bus *pbus, unsigned int devfn)
{
    struct pci_device *pdev;

    pci_for_each_device(pdev, pbus) {
        if (pdev->devfn == devfn)
            return pdev;
    }

    return NULL;
}

struct pci_bus *pci_find_bus(unsigned int domain, unsigned int bus)
{
    return NULL;
}

struct pci_device *pci_find_device(const struct pci_device_id *id, struct pci_device *form)
{
    return NULL;
}
