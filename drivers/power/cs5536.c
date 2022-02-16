/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "cs5536-power"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <delay.h>
#include <asm/regs.h>

static void cs5536_restart(struct power_device *pdev)
{
    msr_set(0x51400017, 1);
    udelay(100);
}

static struct power_ops cs5536_ops = {
    .restart = cs5536_restart,
};

static state cs5536_power_probe(struct pci_device *pdev, const void *pdata)
{
    struct power_device *power;

    power = dev_kzalloc(&pdev->dev, sizeof(*power), GFP_KERNEL);
    if (!power)
        return -ENOMEM;

    power->ops = &cs5536_ops;
    power->dev = &pdev->dev;
    return power_register(power);
}

static const struct pci_device_id cs5536_power_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_AMD_CS5536_ISA) },
    { }, /* NULL */
};

static struct pci_driver cs5536_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = cs5536_power_ids,
    .probe = cs5536_power_probe,
};

static state cs5536_power_init(void)
{
    return pci_driver_register(&cs5536_power_driver);
}
driver_initcall(cs5536_power_init);
