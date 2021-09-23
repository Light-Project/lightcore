/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "platform: " fmt

#include <string.h>
#include <export.h>
#include <mm.h>
#include <driver/platform.h>
#include <initcall.h>
#include "base.h"
#include <printk.h>

static __always_inline bool
platform_device_match_one(const struct platform_device_id *id,
                          struct platform_device *pdev)
{
    return !strcmp(id->name, pdev->name);
}

static inline const struct platform_device_id *
platform_device_match(struct platform_driver *pdrv,
                      struct platform_device *pdev)
{
    const struct platform_device_id *pids = pdrv->platform_table;

    if(!pids || !pdev->name)
        return NULL;

    while(pids->name) {
        if(platform_device_match_one(pids, pdev))
            return pids;
        ++pids;
    }

    return NULL;
}

static state platform_match(struct device *dev, struct driver *drv)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(drv);

    if (platform_device_match(pdrv, pdev))
        return -ENOERR;

#ifdef CONFIG_DT
    if (platform_dt_match(pdrv, pdev))
        return -ENOERR;
#endif

#ifdef CONFIG_ACPI
    if (platform_acpi_match(pdrv, pdev))
        return -ENOERR;
#endif

    return -ENODEV;
}

static state platform_probe(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (unlikely(!pdrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    return pdrv->probe(pdev);
}

static state platform_remove(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->remove)
        return -ENXIO;

    pdrv->remove(pdev);
    return -ENOERR;
}

static state platform_shutdown(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->shutdown)
        return -ENXIO;

    pdrv->shutdown(pdev);
    return -ENOERR;
}

struct bus_type platform_bus = {
    .name = "platform",
    .match = platform_match,
    .probe = platform_probe,
    .remove = platform_remove,
    .shutdown = platform_shutdown,
};

/**
 * platform_alloc_device - Allocating for a new platform device
 * @name: device name
 * @id: device id
 */
struct platform_device *platform_device_alloc(const char *name, int id)
{
    struct platform_device *pdev;

    pdev = kzalloc(sizeof(*pdev), GFP_KERNEL);
    if (!pdev)
        return NULL;

    pdev->name  = name;
    pdev->id    = id;
    pdev->device.bus = &platform_bus;
    return pdev;
}

/**
 * platform_name_resource - Find resource in platform device by name
 * @pdev: device to find
 * @name: match name
 */
struct resource *platform_name_resource(struct platform_device *pdev, const char *name)
{
    struct resource *tmp;
    unsigned int count;

    for (count = 0; count < pdev->resources_nr; ++count) {
        tmp = &pdev->resource[count];
        if (!strcmp(tmp->name, name))
            return tmp;
    }

    return NULL;
}

/**
 * platform_device_add - register a device to the platform bus
 * @pdev: added device
 */
state platform_device_register(struct platform_device *pdev)
{
    pdev->device.bus = &platform_bus;
    return device_register(&pdev->device);
}
EXPORT_SYMBOL(platform_device_register);

void platform_device_unregister(struct platform_device *pdev)
{
    device_unregister(&pdev->device);
}
EXPORT_SYMBOL(platform_device_unregister);

state platform_driver_register(struct platform_driver *pdrv)
{
    pdrv->driver.bus = &platform_bus;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_register);

void platform_driver_unregister(struct platform_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_unregister);

void __init platform_bus_init(void)
{
    bus_register(&platform_bus);

#ifdef CONFIG_DT
    platform_dt_init();
#endif
}