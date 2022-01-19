/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "bus"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device/bus.h>
#include <device.h>
#include <export.h>
#include <printk.h>

static LIST_HEAD(bus_list);

/**
 * bus_add_device - add a device into bus
 * @drv: driver to bind
 */
state bus_add_device(struct device *dev)
{
    struct bus_type *bus;
    state ret;

    if (unlikely(!(bus = dev->bus))) {
        pr_err("device %s without bus\n", dev->name);
        return -EINVAL;
    }

    pr_debug("%s add device %s\n", bus->name, dev->name);
    list_add_prev(&bus->devices_list, &dev->bus_list_device);

    if (bus->autoprobe) {
        ret = device_bind(dev);
        if (unlikely(ret))
            return ret;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(bus_add_device);

void bus_remove_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    pr_debug("%s remove device %s\n", bus->name, dev->name);
    list_del(&dev->bus_list_device);
}
EXPORT_SYMBOL(bus_remove_device);

/**
 * bus_add_driver - add a driver into bus
 * @drv: driver
 */
state bus_add_driver(struct driver *drv)
{
    struct bus_type *bus;
    state ret;

    if (unlikely(!(bus = drv->bus))) {
        pr_err("driver %s without bus\n", drv->name);
        return -EINVAL;
    }

    pr_debug("%s add driver %s\n", bus->name, drv->name);
    list_add_prev(&bus->drivers_list, &drv->bus_list_driver);

    if (bus->autoprobe) {
        ret = driver_bind(drv);
        if (unlikely(ret))
            return ret;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(bus_add_driver);

void bus_remove_driver(struct driver *drv)
{
    list_del(&drv->bus_list_driver);
}
EXPORT_SYMBOL(bus_remove_driver);

/**
 * bus_register - register a driver bus-type
 * @bus: bus to register
 */
state bus_register(struct bus_type *bus)
{
    if (!bus->match || !bus->probe)
        return -EINVAL;

    pr_debug("register %s bus\n", bus->name);

    bus->autoprobe = true;
    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);

    return -ENOERR;
}
EXPORT_SYMBOL(bus_register);

/**
 * bus_register - remove a driver bus-type
 * @bus: bus to register
 */
void bus_unregister(struct bus_type *bus)
{
    pr_debug("unregister %s\n", bus->name);
}
EXPORT_SYMBOL(bus_unregister);

void __init bus_init(void)
{

}
