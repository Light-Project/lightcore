/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"bus: " fmt

#include <device.h>
#include "base.h"
#include <printk.h>

static __always_inline state
device_probe(struct device *dev)
{
    return dev->bus->probe(dev);
}

state driver_probe_device(struct driver *drv, 
                          struct device *dev)
{
    state ret;
    ret = device_probe(dev);
    return ret;
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
    }

    if (ret)
        return -ENODEV;

    dev->driver = drv;

    return -ENOERR;
}

/**
 * device_bind - try to attach device to a driver
 * @drv: driver
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
