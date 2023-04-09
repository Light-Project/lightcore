/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pvpanic-platform"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include "pvpanic.h"
#include <ioops.h>
#include <initcall.h>
#include <driver/platform.h>

static state pvpanic_platform_probe(struct platform_device *pdev, const void *pdata)
{
    struct pvpanic_device *pvdev;
    resource_size_t pmio;
    void *base;

    switch (platform_resource_type(pdev, 0)) {
        case RESOURCE_MMIO:
            base = platform_resource_ioremap(pdev, 0);
            if (!base)
                return -ENOMEM;
            break;

        case RESOURCE_PMIO:
            pmio = platform_resource_start(pdev, 0);
            base = mpio_map(pmio, 1);
            if (!base)
                return -ENOMEM;
            break;

        default:
            return -EINVAL;
    }

    pvdev = platform_kzalloc(pdev, sizeof(*pvdev), GFP_KERNEL);
    if (!pvdev)
        return -ENOMEM;

    pvdev->events = PVPANIC_PANICKED | PVPANIC_CRASH_LOADED;
    pvdev->capability = pvdev->events;

    pvdev->events &= mpio_readb(base);
    pvdev->capability = pvdev->events;

    pvdev->base = base;
    pvdev->dev = &pdev->dev;

    return pvpanic_probe(pvdev);
}

static const struct acpi_device_id pvpanic_acpi_id[] = {
    { "QEMU0001", 0 },
    { }, /* NULL */
};

static const struct dt_device_id pvpanic_dt_id[] = {
    { .compatible = "qemu,pvpanic-mmio" },
    { }, /* NULL */
};

static struct platform_driver pvpanic_platform_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .acpi_table = pvpanic_acpi_id,
    .dt_table = pvpanic_dt_id,
    .probe = pvpanic_platform_probe,
};

static state pvpanic_platform_init(void)
{
    return platform_driver_register(&pvpanic_platform_driver);
}
driver_initcall(pvpanic_platform_init);
