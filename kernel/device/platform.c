/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "platform"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <driver/platform.h>
#include <export.h>
#include <printk.h>

static struct kcache *platform_device_cache;
static struct kcache *platform_devid_cache;

const struct platform_device_id *
platform_device_match(const struct platform_device_id *table, const char *name)
{
    if (!table || !name)
        return NULL;

    while (*table->name) {
        if (!strncmp(table->name, name, PLATFORM_NAME_LEN))
            return table;
        ++table;
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
    dt = platform_dt_match(pdrv->dt_table, pdev->dt_node);
    if (dt) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)dt->data);
        return -ENOERR;
    }
#endif

#ifdef CONFIG_ACPI
    const struct acpi_device_id *acpi;
    acpi = platform_acpi_match(pdrv->acpi_table, pdev->acpi_node);
    if (acpi) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)acpi->data);
        return -ENOERR;
    }
#endif

    platform = platform_device_match(pdrv->platform_table, pdev->name);
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
    if (index >= pdev->resources_nr)
        return 0;
    return pdev->resource[index].start;
}
EXPORT_SYMBOL(platform_resource_start);

resource_size_t platform_resource_size(struct platform_device *pdev, unsigned int index)
{
    if (index >= pdev->resources_nr)
        return 0;
    return pdev->resource[index].size;
}
EXPORT_SYMBOL(platform_resource_size);

resource_size_t platform_resource_end(struct platform_device *pdev, unsigned int index)
{
    if (index >= pdev->resources_nr)
        return 0;
    return resource_end(&pdev->resource[index]);
}
EXPORT_SYMBOL(platform_resource_end);

enum resource_type platform_resource_type(struct platform_device *pdev, unsigned int index)
{
    if (index >= pdev->resources_nr)
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
    struct resource *res;
    unsigned int count;

    for (count = 0; count < pdev->resources_nr; ++count) {
        res = &pdev->resource[count];
        if (!strcmp(res->name, name))
            return res;
    }

    return NULL;
}
EXPORT_SYMBOL(platform_name_resource);

void *platform_resource_ioremap(struct platform_device *pdev, unsigned int index)
{
    struct resource *res = platform_get_resource(pdev, index, RESOURCE_MMIO);
    return platform_ioremap_resource(pdev, res);
}
EXPORT_SYMBOL(platform_resource_ioremap);

void *platform_resource_name_ioremap(struct platform_device *pdev, const char *name)
{
    struct resource *res = platform_name_resource(pdev, name);
    return platform_ioremap_resource(pdev, res);
}
EXPORT_SYMBOL(platform_resource_name_ioremap);

/**
 * platform_alloc_device - allocating for a new platform device.
 * @name: device name.
 * @index: device index.
 */
struct platform_device *platform_device_alloc(const char *name, unsigned int index)
{
    struct platform_device *pdev;

    pdev = kcache_zalloc(platform_device_cache, GFP_KERNEL);
    if (!pdev)
        return NULL;

    pdev->name = name;
    pdev->index = index;

    return pdev;
}
EXPORT_SYMBOL(platform_device_alloc);

void platform_device_free(struct platform_device *pdev)
{
    kcache_free(platform_device_cache, pdev);
}
EXPORT_SYMBOL(platform_device_free);

/**
 * platform_device_register - register a device to the platform bus.
 * @pdev: registering platform device.
 */
state platform_device_register(struct platform_device *pdev)
{
    pdev->dev.bus = &platform_bus;
    return device_register(&pdev->dev);
}
EXPORT_SYMBOL(platform_device_register);

/**
 * platform_device_unregister - unregister a device to the platform bus.
 * @pdev: unregistering platform device.
 */
void platform_device_unregister(struct platform_device *pdev)
{
    device_unregister(&pdev->dev);
}
EXPORT_SYMBOL(platform_device_unregister);

/**
 * platform_driver_register - register a driver to the platform bus.
 * @pdev: registering platform driver.
 */
state platform_driver_register(struct platform_driver *pdrv)
{
    pdrv->driver.bus = &platform_bus;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_register);

/**
 * platform_driver_unregister - unregister a driver to the platform bus.
 * @pdev: unregistering platform driver.
 */
void platform_driver_unregister(struct platform_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}
EXPORT_SYMBOL(platform_driver_unregister);

/**
 * platform_unified_register - register driver and create corresponding device.
 * @pdrv: the platform driver structure.
 * @reg: the platform driver resources.
 * @nres: the platform driver resource number.
 */
struct platform_device *platform_unified_register(struct platform_driver *pdrv, struct resource *res, unsigned int nres)
{
    struct platform_device_id *pids;
    struct platform_device *pdev;
    state ret;

    pdev = platform_device_alloc(pdrv->driver.name, 0);
    if (!pdev)
        return NULL;

    pids = kcache_alloc(platform_devid_cache, GFP_KERNEL);
    if (!pids)
        goto error_pid;

    pdev->resource = res;
    pdev->resources_nr = nres;
    pdev->dev.name = pdrv->driver.name;

    strlcpy(pids[0].name, pdrv->driver.name, PLATFORM_NAME_LEN);
    memset(&pids[1], 0, sizeof(struct platform_device_id));
    pdrv->platform_table = pids;

    ret = platform_device_register(pdev);
    if (ret)
        goto error_pdev;

    ret = platform_driver_register(pdrv);
    if (ret)
        goto error_pdrv;

    return pdev;

error_pdrv:
    platform_device_unregister(pdev);
error_pdev:
    kcache_free(platform_devid_cache, pids);
error_pid:
    kcache_free(platform_device_cache, pdev);
    return NULL;
}
EXPORT_SYMBOL(platform_unified_register);

void __init platform_bus_init(void)
{
    platform_device_cache = kcache_create(
        "platform device",
        sizeof(struct platform_device),
        KCACHE_PANIC
    );

    platform_devid_cache = kcache_create(
        "platform devid",
        sizeof(struct platform_device_id) * 2,
        KCACHE_PANIC
    );

    bus_register(&platform_bus);

#ifdef CONFIG_DT
    platform_dt_init();
#endif
}
