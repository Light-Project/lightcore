/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ce4100-power"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <delay.h>
#include <asm/io.h>

static void ce4100_restart(struct power_device *pdev)
{
    unsigned int count;

    for (count = 0; count < 10; ++count) {
        outb(0xcf9, 0x02);
        udelay(50);
    }
}

static struct power_ops ce4100_ops = {
    .restart = ce4100_restart,
};

static state ce4100_power_probe(struct pci_device *pdev, const void *pdata)
{
    struct power_device *power;

    power = dev_kzalloc(&pdev->dev, sizeof(*power), GFP_KERNEL);
    if (!power)
        return -ENOMEM;

    power->ops = &ce4100_ops;
    power->dev = &pdev->dev;
    return power_register(power);
}

static const struct pci_device_id ce4100_power_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CE4100) },
    { }, /* NULL */
};

static struct pci_driver ce4100_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = ce4100_power_ids,
    .probe = ce4100_power_probe,
};

static state ce4100_power_init(void)
{
    return pci_driver_register(&ce4100_power_driver);
}
driver_initcall(ce4100_power_init);
