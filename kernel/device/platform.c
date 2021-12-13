/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "platform: " fmt

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <export.h>
#include <printk.h>

static __always_inline bool
platform_device_match_one(const struct platform_device_id *id, struct platform_device *pdev)
{
    return !strncmp(id->name, pdev->name, PLATFORM_NAME_LEN);
}

static inline const struct platform_device_id *
platform_device_match(struct platform_driver *pdrv, struct platform_device *pdev)
{
    const struct platform_device_id *pids = pdrv->platform_table;

    if (!pdev->name || !pdrv->platform_table)
        return NULL;

    while (pids->name && pids->name[0]) {
        if (platform_device_match_one(pids, pdev))
            return pids;
        ++pids;
    }

    return NULL;
}

static state platform_match(struct device *dev, struct driver *drv)
{
    struct platform_device *pdev = device_to_platform(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(drv);
    const struct platform_device_id *platform;

#ifdef CONFIG_DT
    const struct dt_device_id *dt;
    dt = platform_dt_match(pdrv, pdev);
    if (dt) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)dt->data);
        return -ENOERR;
    }
#endif

#ifdef CONFIG_ACPI
    const struct acpi_device_id *acpi;
    acpi = platform_acpi_match(pdrv, pdev);
    if (acpi) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)acpi->data);
        return -ENOERR;
    }
#endif

    platform = platform_device_match(pdrv, pdev);
    if (platform) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)platform->data);
        return -ENOERR;
    }

    return -ENODEV;
}

static state platform_probe(struct device *dev)
{
    struct platform_device *pdev = device_to_platform(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (unlikely(!pdrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    return pdrv->probe(pdev, device_get_pdata(dev));
}

static state platform_remove(struct device *dev)
{
    struct platform_device *pdev = device_to_platform(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->remove)
        return -ENXIO;

    pdrv->remove(pdev);
    return -ENOERR;
}

static state platform_shutdown(struct device *dev)
{
    struct platform_device *pdev = device_to_platform(dev);
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

resource_size_t platform_resource_start(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return 0;
    return pdev->resource[index].start;
}
EXPORT_SYMBOL(platform_resource_start);

resource_size_t platform_resource_size(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return 0;
    return pdev->resource[index].size;
}
EXPORT_SYMBOL(platform_resource_size);

resource_size_t platform_resource_end(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return 0;
    return resource_end(&pdev->resource[index]);
}
EXPORT_SYMBOL(platform_resource_end);

enum resource_type platform_resource_type(struct platform_device *pdev, unsigned int index)
{
    if (index > pdev->resources_nr)
        return RESOURCE_NONE;
    return resource_type(&pdev->resource[index]);
}
EXPORT_SYMBOL(platform_resource_type);

/**
 * platform_get_resource - get a resource for a device
 * @pdev: platform device
 * @index: resource index
 * @type: resource type
 */
struct resource *platform_get_resource(struct platform_device *pdev, unsigned int index,
                                       enum resource_type type)
{
    struct resource *res;
    unsigned int count;

    for (count = 0; count < pdev->resources_nr; ++count) {
        res = &pdev->resource[count];
        if (resource_type(res) == type && !index--)
            return res;
    }

    return NULL;
}
EXPORT_SYMBOL(platform_get_resource);

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
EXPORT_SYMBOL(platform_name_resource);

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
    pdev->dev.bus = &platform_bus;
    return pdev;
}
EXPORT_SYMBOL(platform_device_alloc);

/**
 * platform_device_add - register a device to the platform bus
 * @pdev: registering platform device
 */
state platform_device_register(struct platform_device *pdev)
{
    pdev->dev.bus = &platform_bus;
    return device_register(&pdev->dev);
}
EXPORT_SYMBOL(platform_device_register);

/**
 * platform_device_add - unregister a device to the platform bus
 * @pdev: unregistering platform device
 */
void platform_device_unregister(struct platform_device *pdev)
{
    device_unregister(&pdev->dev);
}
EXPORT_SYMBOL(platform_device_unregister);

/**
 * platform_driver_register - register a driver to the platform bus
 * @pdev: registering platform driver
 */
state platform_driver_register(struct platform_driver *pdrv)
{
    pdrv->driver.bus = &platform_bus;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_register);

/**
 * platform_driver_unregister - unregister a driver to the platform bus
 * @pdev: unregistering platform driver
 */
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
