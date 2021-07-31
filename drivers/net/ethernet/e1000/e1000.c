/* SPDX-License-Identifier: GPL-2.0-or-later */
#define DRIVER_NAME "net-e1000"
#undef pr_fmt
#define pr_fmt(fmt)     DRIVER_NAME ": " fmt

#include <driver/pci.h>
#include <init/initcall.h>

#include "e1000_hw.h"

static char e1000_driver_string[] = "Intel(R) PRO/1000 Network Driver";
static const char e1000_copyright[] = "Copyright (c) 1999-2006 Intel Corporation.";

void e1000_pci_set_mwi(struct e1000_hw *hw)
{
    struct pci_device *pdev = hw->back;
    pci_mwi_set(pdev);
}

void e1000_pci_clear_mwi(struct e1000_hw *hw)
{
    struct pci_device *pdev = hw->back;
    pci_mwi_clear(pdev);
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

#define INTEL_E1000_ETHERNET_DEVICE(device_id) {            \
        .vendor = PCI_VENDOR_ID_INTEL, .device = device_id, \
        .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID }

static struct pci_device_id e1000_pci_tbl[] = {
	INTEL_E1000_ETHERNET_DEVICE(0x1000),
	INTEL_E1000_ETHERNET_DEVICE(0x1001),
	INTEL_E1000_ETHERNET_DEVICE(0x1004),
	INTEL_E1000_ETHERNET_DEVICE(0x1008),
	INTEL_E1000_ETHERNET_DEVICE(0x1009),
	INTEL_E1000_ETHERNET_DEVICE(0x100C),
	INTEL_E1000_ETHERNET_DEVICE(0x100D),
	INTEL_E1000_ETHERNET_DEVICE(0x100E),
	INTEL_E1000_ETHERNET_DEVICE(0x100F),
	INTEL_E1000_ETHERNET_DEVICE(0x1010),
	INTEL_E1000_ETHERNET_DEVICE(0x1011),
	INTEL_E1000_ETHERNET_DEVICE(0x1012),
	INTEL_E1000_ETHERNET_DEVICE(0x1013),
	INTEL_E1000_ETHERNET_DEVICE(0x1014),
	INTEL_E1000_ETHERNET_DEVICE(0x1015),
	INTEL_E1000_ETHERNET_DEVICE(0x1016),
	INTEL_E1000_ETHERNET_DEVICE(0x1017),
	INTEL_E1000_ETHERNET_DEVICE(0x1018),
	INTEL_E1000_ETHERNET_DEVICE(0x1019),
	INTEL_E1000_ETHERNET_DEVICE(0x101A),
	INTEL_E1000_ETHERNET_DEVICE(0x101D),
	INTEL_E1000_ETHERNET_DEVICE(0x101E),
	INTEL_E1000_ETHERNET_DEVICE(0x1026),
	INTEL_E1000_ETHERNET_DEVICE(0x1027),
	INTEL_E1000_ETHERNET_DEVICE(0x1028),
	INTEL_E1000_ETHERNET_DEVICE(0x1075),
	INTEL_E1000_ETHERNET_DEVICE(0x1076),
	INTEL_E1000_ETHERNET_DEVICE(0x1077),
	INTEL_E1000_ETHERNET_DEVICE(0x1078),
	INTEL_E1000_ETHERNET_DEVICE(0x1079),
	INTEL_E1000_ETHERNET_DEVICE(0x107A),
	INTEL_E1000_ETHERNET_DEVICE(0x107B),
	INTEL_E1000_ETHERNET_DEVICE(0x107C),
	INTEL_E1000_ETHERNET_DEVICE(0x108A),
	INTEL_E1000_ETHERNET_DEVICE(0x1099),
	INTEL_E1000_ETHERNET_DEVICE(0x10B5),
	INTEL_E1000_ETHERNET_DEVICE(0x2E6E),
    { } /* NULL */
};

static state e1000_probe(struct pci_device *pdev, int pid)
{
    // struct e1000_hw *hw;

    // e1000_reset_hw(hw);

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
	pr_info("%s\n", e1000_driver_string);
	pr_info("%s\n", e1000_copyright);

    return pci_driver_register(&e1000_driver);
}

driver_initcall(e1000_init);
