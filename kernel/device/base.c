/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "dbase"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <export.h>
#include <printk.h>

static state call_bus_probe(struct driver *drv, struct device *dev)
{
    struct bus_type *bus = drv->bus;
    state retval;

    retval = bus->probe(dev);
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
    }

    return retval;
}

static state driver_probe_device(struct driver *drv, struct device *dev)
{
    struct bus_type *bus = dev->bus;
    state retval;

    pr_debug("%s probing driver %s with device %s\n",
            bus->name, drv->name, dev->name);

    if (dev->driver) {
        pr_err("%s bind driver before probing\n", dev->name);
        return -EBUSY;
    }

    if (!list_check_empty(&dev->devres)) {
        pr_crit("%s resources present before probing", dev->name);
        return -EBUSY;
    }

    dev->driver = drv;

    retval = call_bus_probe(drv, dev);
    if (unlikely(retval))
        goto error_probe;

    return -ENOERR;

error_probe:
    devres_release_all(dev);
    return retval;
}

static state device_match_driver(struct device *dev, struct driver *drv)
{
    struct bus_type *bus = dev->bus;
    state retval;

    retval = bus->match(dev, drv);
    if (retval == -ENODEV) {
        /* no match */
        return -ENOERR;
    } else if (retval < 0) {
        pr_err("unexpected (%d) while %s match device %s\n",
                retval, bus->name, dev->name);
        return retval;
    }

    return driver_probe_device(drv, dev);
}

static state driver_match_device(struct driver *drv, struct device *dev)
{
    struct bus_type *bus = dev->bus;
    state retval;

    retval = bus->match(dev, drv);
    if (retval == -ENODEV) {
        /* no match */
        return -ENOERR;
    } else if (retval < 0) {
        pr_err("unexpected (%d) while %s match device %s\n",
                retval, bus->name, dev->name);
        return retval;
    }

    driver_probe_device(drv, dev);
    return -ENOERR;
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
        pr_err("device %s without bus or driver\n", dev->name);
        return -EINVAL;
    }

    device_lock(dev);

    bus_for_each_driver(drv, bus) {
        retval = device_match_driver(dev, drv);
        if (retval)
            goto exit;
    }

exit:
    device_lock(dev);
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
        pr_err("driver %s without bus\n", drv->name);
        return -EINVAL;
    }

    bus_for_each_device(dev, bus) {
        retval = driver_match_device(drv, dev);
        if (retval)
            return retval;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(driver_bind);

static void device_release(struct device *dev)
{

}

/**
 * device_unbind - unbind driver from a specific device
 * @dev: device to unbind from driver
 */
void device_unbind(struct device *dev)
{
    device_release(dev);
}
EXPORT_SYMBOL(device_unbind);

/**
 * driver_unbind - unbind driver from all devices it controls
 * @dev: driver to unbind from device
 */
void driver_unbind(struct driver *drv)
{
    // struct bus_type *bus = drv->bus;
    struct device *dev, *next;
    // state ret;

    driver_for_each_device_safe(dev, next, drv) {



    }
}
EXPORT_SYMBOL(driver_unbind);
