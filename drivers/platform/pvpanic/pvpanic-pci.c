/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pvpanic-pci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include "pvpanic.h"
#include <ioops.h>
#include <initcall.h>
#include <driver/pci.h>

static state pvpanic_pci_probe(struct pci_device *pdev, const void *pdata)
{
    struct pvpanic_device *pvdev;
    void *base;

    pci_device_enable(pdev);

    base = pci_resource_ioremap(pdev, 0, 0);
    if (!base)
        return -ENOMEM;

    pvdev = pci_kzalloc(pdev, sizeof(*pdev), GFP_KERNEL);
    if (!base)
        return -ENOMEM;

    pvdev->events = PVPANIC_PANICKED | PVPANIC_CRASH_LOADED;
    pvdev->capability = pvdev->events;

    pvdev->events &= mpio_readb(base);
    pvdev->capability = pvdev->events;

    pvdev->base = base;
    pvdev->dev = &pdev->dev;

    return pvpanic_probe(pvdev);
}

static const struct pci_device_id pvpanic_pci_id[] = {
    { PCI_DEVICE(0x1b36, 0x0011) },
    { }, /* NULL */
};

static struct pci_driver pvpanic_pci_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = pvpanic_pci_id,
    .probe = pvpanic_pci_probe,
};

static state pvpanic_pci_init(void)
{
    return pci_driver_register(&pvpanic_pci_driver);
}
driver_initcall(pvpanic_pci_init);
