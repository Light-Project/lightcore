/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "pci-res"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/pci.h>
#include <export.h>

state pci_resource_enable(struct pci_device *pdev, uint16_t bars)
{
    struct resource *res;
    unsigned int count;
    uint16_t cmd, ocmd;

    cmd = pci_config_readw(pdev, PCI_COMMAND);
    ocmd = cmd;

    for (count = 0; count < PCI_RESOURCE_MAX; ++count) {
        if (!(bars & BIT(count)))
            continue;

        res = &pdev->resource[count];
        if (!(res->type & (RESOURCE_MMIO | RESOURCE_PMIO)))
            continue;
        if (count == PCI_ROM_RESOURCE && !(res->type & RESOURCE_PCI_ROM_ENABLE))
            continue;

        if (res->type & RESOURCE_MMIO)
            cmd |= PCI_COMMAND_MEMORY;
        if (res->type & RESOURCE_PMIO)
            cmd |= PCI_COMMAND_IO;
    }

    if (ocmd != cmd) {
        pci_info(pdev, "enable resource (%04x -> %04x)\n", ocmd, cmd);
        pci_config_writew(pdev, PCI_COMMAND, cmd);
    }

    return -ENOERR;
}
EXPORT_SYMBOL(pci_resource_enable);
