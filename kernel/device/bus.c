/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"bus: " fmt

#include <printk.h>
#include "base.h"

LIST_HEAD(bus_list);

state bus_device_probe(struct device *dev)
{
    struct bus_type *bus = dev->bus;

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
    list_add(&bus->devices_list, &dev->bus_list_device);
    return device_bind(dev);
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
    list_add(&bus->drivers_list, &drv->bus_list_driver);
    return driver_bind(drv);
}

void bus_driver_remove(struct driver *drv)
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
