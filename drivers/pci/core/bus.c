/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <device.h>
#include <driver/pci.h>
#include <printk.h>

/**
 * pci_bus_device_start - Probe a single device on this PCI bus 
 * 
 */
void pci_bus_device_probe(struct pci_device *pdev)
{
    /* Now we can bind device to the driver */
    device_bind(&pdev->dev);
    pdev->flags_added = true;
}

/**
 * pci_bus_devices_init - Start all devices on this PCI bus
 * @bus: bus to check for new devices
 */
void pci_bus_devices_probe(const struct pci_bus *bus)
{
    struct pci_device *pdev;
    struct pci_bus *bridge;

    pci_bus_for_each_pci_device(pdev, bus) {
        /* Skip already-added devices */
        if(pdev->flags_added)
            continue;
        pci_bus_device_probe(pdev);
    }    
    
    pci_bus_for_each_pci_device(pdev, bus) {
        /* Skip if device attach failed */
        if(!pdev->flags_added)
            continue;
        bridge = pdev->bridge;
        if(bridge)
            pci_bus_devices_probe(bridge);
    }
};

