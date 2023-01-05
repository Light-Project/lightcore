/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "dbase"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <printk.h>
#include <export.h>

static void device_cleanup(struct device *dev)
{
    devres_release_all(dev);
	device_set_pdata(dev, NULL);
    dev->driver = NULL;
}

static state call_device_match(struct driver *drv, struct device *dev)
{
    struct bus_type *bus = dev->bus;
    state retval;

    retval = bus->match(dev, drv);
    if (!retval)
        return true;

    switch (retval) {
        case -ENODEV: case -EINVAL:
            /* no match */
            retval = -ENOERR;
            break;

        default:
            pr_err("%s match %s failed with error (%d)\n",
                    drv->name, dev->name, retval);
            break;
    }

    return retval;
}

static state call_device_probe(struct driver *drv, struct device *dev)
{
    state retval;

    if (dev->bus->probe)
        retval = dev->bus->probe(dev);
    else if (drv->probe)
        retval = drv->probe(dev);
    else
        return -ENXIO;

    if (likely(!retval))
        return -ENOERR;

    switch (retval) {
        case -ENODEV: case -ENXIO:
            pr_debug("%s probe %s rejects match (%d)\n",
                    drv->name, dev->name, retval);
            break;

        case -ENOMEM:
            pr_warn("%s probe %s no memory\n",
                    drv->name, dev->name);
            break;

        default:
            pr_err("%s probe %s failed with error (%d)\n",
                    drv->name, dev->name, retval);
            break;
    }

    return retval;
}

static state call_device_remove(struct driver *drv, struct device *dev)
{
    state retval = -ENOERR;

    if (dev->bus->remove)
        retval = dev->bus->remove(dev);
    else if (drv->remove)
        drv->remove(dev);
    else
        return -ENXIO;

    return retval;
}

static state driver_probe_device(struct driver *drv, struct device *dev)
{
    state retval;

    pr_debug("probing driver %s with device %s\n", drv->name, dev->name);

    if (unlikely(dev->driver)) {
        dev_err(dev, "bind driver before probing\n");
        return -EBUSY;
    }

    if (unlikely(!list_check_empty(&dev->devres))) {
        dev_crit(dev, "resources present before probing\n");
        return -EBUSY;
    }

    dev->driver = drv;
    retval = call_device_probe(drv, dev);
    if (unlikely(retval))
        goto error_probe;

    return -ENOERR;

error_probe:
    devres_release_all(dev);
    return retval;
}

static void driver_remove_device(struct driver *drv, struct device *dev)
{
    call_device_remove(drv, dev);
    device_cleanup(dev);
}

/**
 * device_bind - try to bind device to driver
 * @dev: device to bind driver
 */
state device_bind(struct device *dev)
{
    struct bus_type *bus;
    struct driver *drv;
    state retval = -ENOERR;

    if (unlikely(!(bus = dev->bus))) {
        pr_err("device-bind: device %s without bus or driver\n", dev->name);
        return -EINVAL;
    }

    device_lock(dev);

    bus_for_each_driver(drv, bus) {
        retval = call_device_match(drv, dev);
        if (retval < 0)
            goto exit;
        else if (retval == true) {
            retval = driver_probe_device(drv, dev);
            if (retval)
                goto exit;
        }
    }

exit:
    device_unlock(dev);
    return retval;
}
EXPORT_SYMBOL(device_bind);

/**
 * driver_bind - try to bind driver to devices
 * @drv: driver to bind devices
 */
state driver_bind(struct driver *drv)
{
    struct bus_type *bus;
    struct device *dev;
    state retval;

    if (unlikely(!(bus = drv->bus))) {
        pr_err("driver-bind: driver %s without bus\n", drv->name);
        return -EINVAL;
    }

    bus_for_each_device(dev, bus) {
        retval = call_device_match(drv, dev);
        if (retval < 0)
            return retval;
        else if (retval == true) {
            device_parent_lock(dev);
            driver_probe_device(drv, dev);
            device_parent_unlock(dev);
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(driver_bind);

/**
 * device_unbind - unbind driver from a specific device
 * @dev: device to unbind from driver
 */
void device_unbind(struct device *dev)
{
    struct driver *drv;

    if (unlikely(!(drv = dev->driver))) {
        pr_err("device-unbind: device %s without driver\n", drv->name);
        return;
    }

    device_parent_lock(dev);
    if (likely(drv == dev->driver))
        driver_remove_device(drv, dev);
    device_parent_unlock(dev);
}
EXPORT_SYMBOL(device_unbind);

/**
 * driver_unbind - unbind driver from all devices it controls
 * @dev: driver to unbind from device
 */
void driver_unbind(struct driver *drv)
{
    struct device *dev, *next;

    driver_for_each_device_safe(dev, next, drv)
        call_device_remove(drv, dev);
}
EXPORT_SYMBOL(driver_unbind);
