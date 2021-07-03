/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * PCI bus system interface
 */

#include <types.h>
#include <export.h>
#include <init/initcall.h>
#include <device/bus.h>
#include <driver/pci.h>
#include <printk.h>

static bool pci_device_match_one(const struct pci_device_id *ids,
                                struct pci_device *pdev)
{
    return ((ids->vendor == PCI_ANY_ID || ids->vendor == pdev->vendor) &&
            (ids->subvendor == PCI_ANY_ID || ids->subvendor == pdev->subvendor) &&
            (ids->device == PCI_ANY_ID || ids->device == pdev->device) &&
            (ids->subdevice == PCI_ANY_ID || ids->subdevice == pdev->subdevice) &&
            !((ids->class ^ pdev->class) & ids->class_mask)
    );
}

/**
 * pci_device_match - See if a device matches a driver's list of IDs
 * 
 */
static struct pci_device_id *pci_device_match(struct pci_driver *pdrv, 
                                    struct pci_device *pdev)
{
    const struct pci_device_id *pids = pdrv->id_table;
    while(pids->vendor || pids->subvendor || pids->class_mask) {
        if(pci_device_match_one(pids, pdev))
            return (struct pci_device_id *)pids;
        ++pids;
    }
    return NULL;
}

/**
 * pci_driver_register - Register a new pci device
 * @drv: the pci driver struct to register
 * 
 * @return state 
 */
state pci_driver_register(struct pci_driver *pdrv)
{
    pdrv->driver.bus = &pci_bus_type;

    /* Register to core */
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(pci_driver_register);

void pci_driver_unregister(struct pci_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}

static state device_probe(struct pci_driver *pdrv, struct pci_device *pdev, 
                        struct pci_device_id *id)
{
    return pdrv->probe(pdev, id->driver_data);
}

static state device_remove(struct pci_driver *pdrv, struct pci_device *pdev)
{
    return pdrv->remove(pdev);
}

static state pci_bus_match(struct device *dev, struct driver *drv)
{
    struct pci_device *pdev = device_to_pci_device(dev);
    struct pci_driver *pdrv = driver_to_pci_driver(drv);

    if(!pci_device_match(pdrv, pdev))
        return -ENODEV;

    return -ENOERR;
}

static state pci_bus_probe(struct device *dev)
{
    struct pci_device *pdev = device_to_pci_device(dev);
    struct pci_driver *pdrv = driver_to_pci_driver(dev->driver);
    struct pci_device_id *pid;

    /* If pci driver no probe function, exit */
    if(!pdrv->probe)
        return -ENODEV;

    pid = pci_device_match(pdrv, pdev);
    if(!pid)
        return -ENODEV;

    return device_probe(pdrv, pdev, pid);
}

static state pci_bus_remove(struct device *dev)
{
    struct pci_device *pdev = device_to_pci_device(dev);
    struct pci_driver *pdrv = driver_to_pci_driver(dev->driver);

    /* If pci driver no remove function, exit */
    if(!pdrv->remove)
        return -ENODEV;

    return device_remove(pdrv, pdev);
}

struct bus_type pci_bus_type = {
    .name = "pci",
    .match = pci_bus_match,
    .probe = pci_bus_probe,
    .remove = pci_bus_remove,
};

static state pci_bus_init(void)
{
    /* Register pci bus to system */
    return bus_register(&pci_bus_type);
}
core_initcall(pci_bus_init);
