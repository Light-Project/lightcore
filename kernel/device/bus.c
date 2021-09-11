/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"bus: " fmt

#include <device.h>
#include <device/bus.h>
#include "base.h"
#include <printk.h>

state bus_device_probe(struct device *dev)
{
    struct bus_type *bus = dev->bus;
    // state ret;

    mutex_lock(&bus->mutex);

    mutex_unlock(&bus->mutex);
    return -ENOERR;
}

/**
 * bus_device_add - add a device into bus
 * @dev: driver
 */
state bus_device_add(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    if (!bus)
        return -EINVAL;

    pr_debug("%s add device %s\n", bus->name, dev->name);

    device_bind(dev);
    list_add_prev(&bus->devices_list, &dev->bus_list_device);

    return -ENOERR;
}

void bus_device_remove(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    pr_debug("%s remove device %s\n", bus->name, dev->name);

    list_del(&dev->bus_list_device);
}

/**
 * bus_driver_add - add a driver into bus
 * @drv: driver
 */
state bus_driver_add(struct driver *drv)
{
    struct bus_type *bus = drv->bus;

    pr_debug("%s add driver %s\n", bus->name, drv->name);

    list_add_prev(&bus->drivers_list, &drv->bus_list_driver);

    return driver_bind(drv);
}

void bus_driver_remove(struct driver *drv)
{
    list_del(&drv->bus_list_driver);
}

state bus_register(struct bus_type *bus)
{
    pr_debug("register bus %s\n", bus->name);

    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);

    return -ENOERR;
}

void bus_unregister(struct bus_type *bus)
{
    pr_debug("unregister bus %s\n", bus->name);

}

void __init bus_init(void)
{

}
