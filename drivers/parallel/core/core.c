/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <driver/parallel.h>
#include <driver/platform.h>
#include <export.h>

static struct kcache *parallel_device_cache;
SPIN_LOCK(parallel_host_lock);
LIST_HEAD(parallel_host_list);

static state parallel_match(struct device *dev, struct driver *drv)
{
    struct parallel_device *pdev = device_to_parallel_device(dev);
    struct parallel_driver *pdrv = driver_to_parallel_driver(drv);
    const struct platform_device_id *platform;

#ifdef CONFIG_PARALLEL_DT
    const struct dt_device_id *dt;
    dt = platform_dt_match(pdrv->dt_table, pdev->dt_node);
    if (dt) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)dt->data);
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

static state parallel_probe(struct device *dev)
{
    struct parallel_device *pdev = device_to_parallel_device(dev);
    struct parallel_driver *pdrv = driver_to_parallel_driver(dev->driver);

    if (unlikely(!pdrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    return pdrv->probe(pdev, device_get_pdata(dev));
}

static state parallel_remove(struct device *dev)
{
    struct parallel_device *pdev = device_to_parallel_device(dev);
    struct parallel_driver *pdrv = driver_to_parallel_driver(dev->driver);

    if (!pdrv->remove)
        return -ENXIO;

    pdrv->remove(pdev);
    return -ENOERR;
}

static state parallel_shutdown(struct device *dev)
{
    struct parallel_device *pdev = device_to_parallel_device(dev);
    struct parallel_driver *pdrv = driver_to_parallel_driver(dev->driver);

    if (!pdrv->shutdown)
        return -ENXIO;

    pdrv->shutdown(pdev);
    return -ENOERR;
}

struct bus_type parallel_bus = {
    .name = "parallel",
    .match = parallel_match,
    .probe = parallel_probe,
    .remove = parallel_remove,
    .shutdown = parallel_shutdown,
};

/**
 * parallel_device_alloc - allocating for a new parallel device.
 * @host: parent host controller of the device.
 * @name: device name for matching.
 */
struct parallel_device *parallel_device_alloc(struct parallel_host *host, const char *name)
{
    struct parallel_device *pdev;

    pdev = kcache_zalloc(parallel_device_cache, GFP_KERNEL);
    if (!pdev)
        return NULL;

    pdev->name = name;
    pdev->host = host;

    return pdev;
}
EXPORT_SYMBOL(parallel_device_alloc);

/**
 * parallel_device_free - release a new parallel device.
 * @pdev: the device to be release.
 */
void parallel_device_free(struct parallel_device *pdev)
{
    kcache_free(parallel_device_cache, pdev);
}
EXPORT_SYMBOL(parallel_device_free);

/**
 * parallel_device_register - register a device to the parallel bus.
 * @pdev: registering parallel device.
 */
state parallel_device_register(struct parallel_device *pdev)
{
    pdev->dev.bus = &parallel_bus;
    return device_register(&pdev->dev);
}
EXPORT_SYMBOL(parallel_device_register);

/**
 * parallel_device_unregister - unregister a device to the parallel bus.
 * @pdev: unregistering parallel device.
 */
void parallel_device_unregister(struct parallel_device *pdev)
{
    device_unregister(&pdev->dev);
}
EXPORT_SYMBOL(parallel_device_unregister);

/**
 * parallel_driver_register - register a driver to the parallel bus.
 * @pdev: registering parallel driver.
 */
state parallel_driver_register(struct parallel_driver *pdrv)
{
    pdrv->driver.bus = &parallel_bus;
    return driver_register(&pdrv->driver);
}
EXPORT_SYMBOL(parallel_driver_register);

/**
 * parallel_driver_unregister - unregister a driver to the parallel bus.
 * @pdev: unregistering parallel driver.
 */
void parallel_driver_unregister(struct parallel_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}
EXPORT_SYMBOL(parallel_driver_unregister);

/**
 * parallel_host_register - register a parallel host controller.
 * @pdev: registering parallel host.
 */
state parallel_host_register(struct parallel_host *host)
{
    if (!host->ops || !host->dev)
        return -EINVAL;

#ifdef CONFIG_PARALLEL_DT
    dt_populate_parallel(host);
#endif

    spin_lock(&parallel_host_lock);
    list_add(&parallel_host_list, &host->list);
    spin_unlock(&parallel_host_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(parallel_host_register);

/**
 * parallel_host_unregister - unregister a parallel host controller.
 * @pdev: unregistering parallel host.
 */
void parallel_host_unregister(struct parallel_host *host)
{
    spin_lock(&parallel_host_lock);
    list_del(&host->list);
    spin_unlock(&parallel_host_lock);
}
EXPORT_SYMBOL(parallel_host_unregister);

static state parallel_init(void)
{
    parallel_device_cache = kcache_create(
        "parallel device",
        sizeof(struct parallel_device),
        KCACHE_PANIC
    );

    return bus_register(&parallel_bus);
}
framework_initcall(parallel_init);
