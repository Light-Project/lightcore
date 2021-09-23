/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"dbase: " fmt

#include <device.h>
#include <printk.h>

#include "base.h"

/**
 * @driver_probe_device - probe a device through a driver
 * @drv: device driver
 * @dev: device to probe
 */
state driver_probe_device(struct driver *drv, struct device *dev)
{
    state ret;

    ret = drv->bus->probe(dev);

    if (ret) {
        devres_release_all(dev);
        return ret;
    }

    return -ENOERR;
}

state device_match(struct device *dev)
{
    struct bus_type *bus = dev->bus;
    struct driver *drv;
    state ret;

    if (!bus->match)
        return -ENXIO;

    bus_for_each_driver(drv, bus) {
        ret = bus->match(dev, drv);
        if(!ret)
            break;
    } if (ret) {
        return -ENODEV;
    }

    dev->driver = drv;
    return -ENOERR;
}

/**
 * device_bind - try to bind device to a driver
 * @dev: device to bind
 */
state device_bind(struct device *dev)
{
    state ret;

    ret = device_match(dev);
    if (ret)
        return ret;

    mutex_lock(&dev->mutex);

    ret = driver_probe_device(dev->driver, dev);

    mutex_unlock(&dev->mutex);
    return -ENOERR;
}

/**
 * driver_bind - try to bind device to drivers
 * @drv: driver
 */
state driver_bind(struct driver *drv)
{
    struct bus_type *bus = drv->bus;
    struct device *dev;
    state ret;

    bus_for_each_device(dev, bus) {
        ret = bus->match(dev, drv);
        if(ret)
            continue;
        dev->driver = drv;
        driver_probe_device(drv, dev);
    }

    return -ENOERR;
}
