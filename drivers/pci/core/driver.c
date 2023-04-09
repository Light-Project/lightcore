/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/pci.h>
#include <export.h>
#include <printk.h>

static __always_inline bool
pci_device_match_one(const struct pci_device_id *id, struct pci_device *pdev)
{
    return ((id->vendor    == PCI_ANY_ID || id->vendor    == pdev->vendor)    &&
            (id->subvendor == PCI_ANY_ID || id->subvendor == pdev->subvendor) &&
            (id->device    == PCI_ANY_ID || id->device    == pdev->device)    &&
            (id->subdevice == PCI_ANY_ID || id->subdevice == pdev->subdevice) &&
            !((id->class ^ pdev->class) & id->class_mask));
}

static const struct pci_device_id *
pci_device_match(struct pci_driver *pdrv, struct pci_device *pdev)
{
    const struct pci_device_id *pids = pdrv->id_table;

    while (pids->vendor || pids->subvendor || pids->class_mask) {
        if (pci_device_match_one(pids, pdev))
            return pids;
        ++pids;
    }

    return NULL;
}

static state pci_real_probe(struct pci_device *pdev, struct pci_driver *pdrv)
{
    return pdrv->probe(pdev, pci_get_devdata(pdev));
}

static state pci_bus_match(struct device *dev, struct driver *drv)
{
    struct pci_device *pdev = device_to_pci(dev);
    struct pci_driver *pdrv = driver_to_pci(drv);
    const struct pci_device_id *pci;

    pci = pci_device_match(pdrv, pdev);
    if (!pci)
        return -ENODEV;

    device_set_pdata(dev, (void *)pci->data);
    return -ENOERR;
}

static state pci_bus_probe(struct device *dev)
{
    struct pci_device *pdev = device_to_pci(dev);
    struct pci_driver *pdrv = driver_to_pci(dev->driver);
    state ret;

    /* If pci driver no probe function, exit */
    if(!pdrv->probe)
        return -ENXIO;

    pci_irq_setup(pdev);
    ret = pcibios_irq_alloc(pdev);
    if (ret)
        return ret;

    ret = pci_real_probe(pdev, pdrv);
    if (!ret)
        return -ENOERR;

    pcibios_irq_free(pdev);
    return ret;
}

static state pci_bus_remove(struct device *dev)
{
    struct pci_device *pdev = device_to_pci(dev);
    struct pci_driver *pdrv = driver_to_pci(dev->driver);

    if (!pdrv->remove)
        return -ENXIO;

    pdrv->remove(pdev);
    pcibios_irq_free(pdev);

    if (pdev->power_state == PCI_POWER_D0)
        pdev->power_state = PCI_POWER_UNKNOWN;

    return -ENOERR;
}

struct bus_type pci_bus_type = {
    .name = "pci",
    .match = pci_bus_match,
    .probe = pci_bus_probe,
    .remove = pci_bus_remove,
};

state pci_driver_register(struct pci_driver *pdrv)
{
    if (!pdrv->id_table)
        return -EINVAL;

    pdrv->driver.bus = &pci_bus_type;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(pci_driver_register);

void pci_driver_unregister(struct pci_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}
EXPORT_SYMBOL(pci_driver_unregister);

static __init state pci_bus_init(void)
{
    /* Register pci bus to system */
    return bus_register(&pci_bus_type);
}
framework_initcall(pci_bus_init);
