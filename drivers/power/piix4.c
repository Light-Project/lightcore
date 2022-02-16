/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "piix4-power"

#include <initcall.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <driver/power/piix4.h>
#include <delay.h>
#include <asm/io.h>

struct piix4_device {
    struct power_device power;
    resource_size_t base;
};

#define power_to_piix4(pdev) \
    container_of(pdev, struct piix4_device, power)

#define PIIX4_SUSPEND_MAGIC 0x00120002

static void piix4_power_shutdown(struct power_device *pdev)
{
    struct piix4_device *piix4 = power_to_piix4(pdev);
    struct pci_device *pci = device_to_pci(pdev->dev);
    uint16_t val;

    for (;;) {
        val = inw(piix4->base + PIIX4_POWER_STS);
        if (!(val & PIIX4_POWER_PMEN_PWRBTN))
            break;
        outw(piix4->base + PIIX4_POWER_STS, val);
    }

    outw(piix4->base + PIIX4_POWER_STS, PIIX4_POWER_CNTRL_SUS_EN);
    mdelay(10);

    pci_config_writel(pci, 0, PIIX4_SUSPEND_MAGIC);
    mdelay(1000);
}

static struct power_ops piix_power_ops = {
    .shutdown = piix4_power_shutdown,
};

static state piix4_power_probe(struct pci_device *pdev, const void *pdata)
{
    struct piix4_device *piix4;

    if (pci_resource_type(pdev, PCI_BRIDGE_RESOURCES) != RESOURCE_PMIO)
        return -ENODEV;

    piix4 = dev_kzalloc(&pdev->dev, sizeof(*piix4), GFP_KERNEL);
    if (!piix4)
        return -ENOMEM;

    piix4->base = pci_resource_start(pdev, PCI_BRIDGE_RESOURCES);
    piix4->power.ops = &piix_power_ops;
    piix4->power.dev = &pdev->dev;
    return power_register(&piix4->power);
}

static struct pci_device_id piix4_power_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82371AB_3) },
    { }, /* NULL */
};

static struct pci_driver piix4_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = piix4_power_ids,
    .probe = piix4_power_probe,
};

static state piix4_power_init(void)
{
    return pci_driver_register(&piix4_power_driver);
}
driver_initcall(piix4_power_init);
