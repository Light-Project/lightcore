/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "bus: " fmt

#include <device/bus.h>
#include <device.h>
#include <printk.h>

LIST_HEAD(bus_list);

/**
 * @bus_probe_device - probe a device through a driver
 * @drv: device driver
 * @dev: device to probe
 */
state bus_probe_device(struct driver *drv, struct device *dev)
{
    state ret;

    pr_debug("%s probing driver %s with device %s\n",
            drv->bus->name, drv->name, dev->name);

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
        return -ENOERR;
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

    if (dev->driver)
        ret = bus_probe_device(dev->driver, dev);

    mutex_unlock(&dev->mutex);
    return ret;
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
        bus_probe_device(drv, dev);
    }

    return -ENOERR;
}

/**
 * bus_add_device - add a device into bus
 * @dev: driver
 */
state bus_add_device(struct device *dev)
{
    struct bus_type *bus;

    if (!(bus = dev->bus))
        return -EINVAL;

    pr_debug("%s add device %s\n", bus->name, dev->name);
    list_add_prev(&bus->devices_list, &dev->bus_list_device);
    return device_bind(dev);
}

void bus_remove_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    pr_debug("%s remove device %s\n", bus->name, dev->name);
    list_del(&dev->bus_list_device);
}

/**
 * bus_add_driver - add a driver into bus
 * @drv: driver
 */
state bus_add_driver(struct driver *drv)
{
    struct bus_type *bus;

    if (!(bus = drv->bus))
        return -EINVAL;

    pr_debug("%s add driver %s\n", bus->name, drv->name);

    list_add_prev(&bus->drivers_list, &drv->bus_list_driver);
    return driver_bind(drv);
}

void bus_remove_driver(struct driver *drv)
{
    list_del(&drv->bus_list_driver);
}

/**
 * bus_register - register a driver bus-type
 * @bus: bus to register
 */
state bus_register(struct bus_type *bus)
{
    if (!bus->match || !bus->probe)
        return -EINVAL;

    pr_debug("register %s\n", bus->name);
    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);
    return -ENOERR;
}

/**
 * bus_register - remove a driver bus-type
 * @bus: bus to register
 */
void bus_unregister(struct bus_type *bus)
{
    pr_debug("unregister %s\n", bus->name);
}

void __init bus_init(void)
{

}
