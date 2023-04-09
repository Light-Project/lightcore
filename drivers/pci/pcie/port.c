/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pcie-port"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/pci.h>

static state pcie_port_probe(struct pci_device *pdev, const void *pdata)
{

    return -ENOERR;
}

static struct pci_device_id pcie_port_ids[] = {
    { PCI_DEVICE_CLASS((PCI_CLASS_BRIDGE_PCI  << 8) | 0x00, ~0) },
    { PCI_DEVICE_CLASS((PCI_CLASS_BRIDGE_PCI  << 8) | 0x01, ~0) },
    { PCI_DEVICE_CLASS((PCI_CLASS_SYSTEM_RCEC << 8) | 0x00, ~0) },
    { }, /* NULL */
};

static struct pci_driver pcie_port_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = pcie_port_ids,
    .probe = pcie_port_probe,
};

static state pcie_port_init(void)
{
    return pci_driver_register(&pcie_port_driver);
}
driver_initcall(pcie_port_init);
