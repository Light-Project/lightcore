/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <driver/isa.h>
#include <export.h>

static struct kcache *isa_cache;

static state isa_bus_match(struct device *dev, struct driver *drv)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(drv);

    if (unlikely(!idrv->match)) {
        pr_warn("match not supported\n");
        return -ENXIO;
    }

    return idrv->match(idev, idev->index);
}

static state isa_bus_probe(struct device *dev)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(dev->driver);

    if (unlikely(!idrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    return idrv->probe(idev, idev->index, device_get_pdata(dev));
}

static state isa_bus_remove(struct device *dev)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(dev->driver);

    if (!idrv->remove)
        return -ENXIO;

    idrv->remove(idev, idev->index);
    return -ENOERR;
}

static state isa_bus_shutdown(struct device *dev)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(dev->driver);

    if (!idrv->shutdown)
        return -ENXIO;

    idrv->shutdown(idev, idev->index);
    return -ENOERR;
}

static state isa_bus_suspend(struct device *dev, pm_message_t msg)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(dev->driver);

    if (!idrv->suspend)
        return -ENXIO;

    return idrv->suspend(idev, idev->index, msg);
}

static state isa_bus_resume(struct device *dev)
{
    struct isa_device *idev = device_to_isa_device(dev);
    struct isa_driver *idrv = driver_to_isa_driver(dev->driver);

    if (!idrv->resume)
        return -ENXIO;

    return idrv->resume(idev, idev->index);
}

static struct device isa_root = {
    .name = "isa-root",
};

static struct bus_type isa_bus = {
    .name = "isa",
    .match = isa_bus_match,
    .probe = isa_bus_probe,
    .remove = isa_bus_remove,
    .shutdown = isa_bus_shutdown,
    .suspend = isa_bus_suspend,
    .resume = isa_bus_resume
};

static void isa_device_release(struct device *dev)
{
    struct isa_device *idev = device_to_isa_device(dev);
    kcache_free(isa_cache, idev);
}

state isa_driver_register(struct isa_driver *idrv, unsigned int num, const void *pdata)
{
    unsigned int count;
    state retval;

    idrv->driver.bus = &isa_bus;
    list_head_init(&idrv->devices);

    retval = driver_register(&idrv->driver);
    if (retval)
        return retval;

    for (count = 0; count < num; ++count) {
        struct isa_device *idev;

        idev = kcache_zalloc(isa_cache, GFP_KERNEL);
        if (unlikely(!idev)) {
            retval = -ENOMEM;
            goto error;
        }

        idev->index = count;
        idev->dev.bus = &isa_bus;
        idev->dev.parent = &isa_root;
        idev->dev.release = isa_device_release;
        isa_set_devdata(idev, (void *)pdata);

        retval = device_register(&idev->dev);
        if (!retval)
            goto error;

        list_add(&idrv->devices, &idev->list);
    }

    return -ENOERR;

error:
    isa_driver_unregister(idrv);
    return retval;
}
EXPORT_SYMBOL(isa_driver_register);

void isa_driver_unregister(struct isa_driver *idrv)
{
    struct isa_device *idev, *tmp;

    list_for_each_entry_safe(idev, tmp, &idrv->devices, list)
        device_unregister(&idev->dev);

    driver_unregister(&idrv->driver);
}
EXPORT_SYMBOL(isa_driver_unregister);

static state isa_bus_init(void)
{
    state retval;

    isa_cache = KCACHE_CREATE(struct isa_device, 0);
    if (!isa_cache)
        return -ENOMEM;

    retval = bus_register(&isa_bus);
    if (retval)
        return retval;

    retval = device_register(&isa_root);
    if (retval)
        bus_unregister(&isa_bus);

    return retval;
}
framework_initcall(isa_bus_init);
