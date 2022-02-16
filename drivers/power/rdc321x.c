/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "rdc321x-power"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <delay.h>
#include <asm/io.h>

static void rdc321x_restart(struct power_device *pdev)
{
    uint32_t val;

    /* Voluntary reset the watchdog timer */
    outl(0xcf8, 0x80003840);

    /* Generate a CPU reset on next tick */
    val = inl(0xcfc);
    val |= 0x1600;
    outl(0xcfc, val);
    outb(0x921, 1);

    udelay(100);
}

static struct power_ops rdc321x_ops = {
    .restart = rdc321x_restart,
};

static state rdc321x_power_probe(struct pci_device *pdev, const void *pdata)
{
    struct power_device *power;

    power = dev_kzalloc(&pdev->dev, sizeof(*power), GFP_KERNEL);
    if (!power)
        return -ENOMEM;

    power->ops = &rdc321x_ops;
    power->dev = &pdev->dev;
    return power_register(power);
}

static const struct pci_device_id rdc321x_power_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_RDC, PCI_DEVICE_ID_RDC_R6030) },
    { }, /* NULL */
};

static struct pci_driver rdc321x_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = rdc321x_power_ids,
    .probe = rdc321x_power_probe,
};

static state rdc321x_power_init(void)
{
    return pci_driver_register(&rdc321x_power_driver);
}
driver_initcall(rdc321x_power_init);
