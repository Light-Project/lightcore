/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

unsigned int pcibios_max_latency = 255;

/**
 * pcibios_master_set -
 *
 */
void __weak pcibios_master_set(struct pci_device *pdev)
{
    uint8_t val;

    if (pci_is_pcie(pdev))
        return;

    val = pci_config_readb(pdev, PCI_LATENCY_TIMER);
    if (val < 16)
        val = (64 <= pcibios_max_latency) ? 64 : pcibios_max_latency;
    else if (val > pcibios_max_latency)
        val = pcibios_max_latency;
    else
        return;
    pci_config_writeb(pdev, PCI_LATENCY_TIMER, val);
}

/**
 * pcibios_bus_add - assign irq when probing device.
 * @pdev: pci bus to add
 */
void __weak pcibios_device_add(struct pci_device *pdev)
{

}
EXPORT_SYMBOL(pcibios_device_add);

/**
 * pcibios_device_remove - assign irq when probing device.
 * @pdev: pci bus to add
 */
void __weak pcibios_device_remove(struct pci_device *pdev)
{

}
EXPORT_SYMBOL(pcibios_device_remove);

/**
 * pcibios_bus_add - assign irq when probing device.
 * @pdev: pci bus to add
 */
void __weak pcibios_bus_add(struct pci_bus *pbus)
{

}
EXPORT_SYMBOL(pcibios_bus_add);

/**
 * pcibios_bus_remove - assign irq when probing device.
 * @pdev: pci bus to remove
 */
void __weak pcibios_bus_remove(struct pci_bus *pbus)
{

}
EXPORT_SYMBOL(pcibios_bus_remove);

/**
 * pcibios_irq_alloc - assign irq when probing device.
 * @pdev: pci deivice to assign irq
 */
state __weak pcibios_irq_alloc(struct pci_device *pdev)
{
    return -ENOERR;
}
EXPORT_SYMBOL(pcibios_irq_alloc);

/**
 * pcibios_irq_free - release irq when removing device.
 * @pdev: pci deivice to release irq
 */
void __weak pcibios_irq_free(struct pci_device *pdev)
{

}
EXPORT_SYMBOL(pcibios_irq_free);
