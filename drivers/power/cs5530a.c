/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "cs5530a-power"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <delay.h>

static void cs5530a_warm_restart(struct power_device *pdev)
{
    struct pci_device *pci = device_to_pci(pdev->dev);
    pci_config_writeb(pci, 0x44, 0x01);
    udelay(100);
}

static struct power_ops cs5530a_ops = {
    .restart = cs5530a_warm_restart,
};

static state cs5530a_power_probe(struct pci_device *pdev, const void *pdata)
{
    struct power_device *power;

    power = dev_kzalloc(&pdev->dev, sizeof(*power), GFP_KERNEL);
    if (!power)
        return -ENOMEM;

    power->ops = &cs5530a_ops;
    power->dev = &pdev->dev;
    return power_register(power);
}

static const struct pci_device_id cs5530a_power_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_CYRIX, PCI_DEVICE_ID_CYRIX_5530_LEGACY) },
    { PCI_DEVICE(PCI_VENDOR_ID_NS, PCI_DEVICE_ID_NS_SC1100_BRIDGE) },
    { }, /* NULL */
};

static struct pci_driver cs5530a_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = cs5530a_power_ids,
    .probe = cs5530a_power_probe,
};

static state cs5530a_power_init(void)
{
    return pci_driver_register(&cs5530a_power_driver);
}
driver_initcall(cs5530a_power_init);
