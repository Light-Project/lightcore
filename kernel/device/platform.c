/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "platform: " fmt

#include <string.h>
#include <export.h>
#include <mm.h>
#include <driver/platform.h> 
#include <init/initcall.h>
#include "base.h"
#include <printk.h>

static __always_inline bool
platform_device_match_one(const struct platform_device_id *id, 
                          struct platform_device *pdev)
{
    return !strcmp(id->name, pdev->name);
}

/**
 * platform_device_match - See if a device matches a driver's list of IDs
 */
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

    if(platform_device_match(pdrv, pdev) ||
       platform_dt_match(pdrv, pdev))
        return -ENOERR;

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

    return pdrv->remove(pdev);
}

static state platform_shutdown(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->shutdown)
        return -ENXIO;

    return pdrv->shutdown(pdev);
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
 * @id: device ID
 */
struct platform_device *platform_device_alloc(const char *name, int id)
{
    struct platform_device *pdev;
    pdev = kzalloc(sizeof(*pdev), GFP_KERNEL);
    pdev->name  = name;
    pdev->id    = id;
    pdev->device.bus = &platform_bus;
    return pdev;
}

/**
 * platform_device_add - register a device to the platform bus
 * @pdev: added device
 */
state platform_device_register(struct platform_device *pdev)
{
    if(!pdev)
        return -EINVAL;

    pdev->device.bus = &platform_bus;
    return device_register(&pdev->device);
}
EXPORT_SYMBOL(platform_device_register);

void platform_device_unregister(struct platform_device *pdev)
{
    if(!pdev)
        return;

}
EXPORT_SYMBOL(platform_device_unregister);

state platform_driver_register(struct platform_driver *pdrv)
{
    if(!pdrv)
        return -EINVAL;

    pdrv->driver.bus = &platform_bus;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_register);

void platform_driver_unregister(struct platform_driver *pdrv)
{
    if(!pdrv)
        return;
    driver_unregister(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_unregister);

void __init platform_bus_init(void)
{
    bus_register(&platform_bus);
    platform_dt_init();
}