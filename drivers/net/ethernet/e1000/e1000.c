/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "net-e1000"
#define pr_fmt(fmt)	DRIVER_NAME ": " fmt

#include <initcall.h>
#include "e1000_hw.h"

void e1000_pci_set_mwi(struct e1000_hw *hw)
{
    struct pci_device *pdev = hw->back;
    pci_mwi_enable(pdev);
}

void e1000_pci_clear_mwi(struct e1000_hw *hw)
{
    struct pci_device *pdev = hw->back;
    pci_mwi_disable(pdev);
}

int e1000_pcix_get_mmrbc(struct e1000_hw *hw)
{
    struct pci_device *pdev = hw->back;
    return pcix_mmrbc_get(pdev);
}

void e1000_pcix_set_mmrbc(struct e1000_hw *hw, int mmrbc)
{
    struct pci_device *pdev = hw->back;
    pcix_mmrbc_set(pdev, mmrbc);
}

void e1000_io_write(struct e1000_hw *hw, unsigned long port, u32 value)
{
    outl(port, value);
}

static struct pci_device_id e1000_pci_tbl[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1000) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1001) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1004) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1008) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1009) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x100C) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x100D) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x100E) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x100F) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1010) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1011) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1012) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1013) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1014) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1015) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1016) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1017) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1018) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1019) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x101A) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x101D) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x101E) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1026) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1027) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1028) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1075) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1076) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1077) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1078) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1079) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x107A) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x107B) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x107C) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x108A) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1099) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x10B5) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2E6E) },
    { } /* NULL */
};

static state e1000_probe(struct pci_device *pdev, const void *pdata)
{
    return -ENOERR;
}

static struct pci_driver e1000_driver = {
    .driver = {
        .name = DRIVER_NAME
    },
    .id_table = e1000_pci_tbl,
    .probe = e1000_probe,
};

static state e1000_init(void)
{
    return pci_driver_register(&e1000_driver);
}
driver_initcall(e1000_init);
