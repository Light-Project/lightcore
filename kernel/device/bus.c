/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "dbus"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device/bus.h>
#include <string.h>
#include <device.h>
#include <printk.h>
#include <export.h>

static LIST_HEAD(bus_list);
static SPIN_LOCK(bus_lock);

static struct bus_type *bus_find_unlock(const char *name)
{
    struct bus_type *walk;

    list_for_each_entry(walk, &bus_list, list) {
        if (!strcmp(walk->name, name))
            return walk;
    }

    return NULL;
}

/**
 * bus_add_device - add a device into bus
 * @drv: driver to bind
 */
state bus_add_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;
    state retval;

    if (unlikely(!bus)) {
        pr_debug("device %s without bus\n", dev->name);
        return -ENOERR;
    }

    pr_debug("%s add device %s\n", bus->name, dev->name);
    list_add_prev(&bus->devices_list, &dev->bus_list);

    if (bus->autoprobe) {
        retval = device_bind(dev);
        if (unlikely(retval))
            return retval;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(bus_add_device);

void bus_remove_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    pr_debug("%s remove device %s\n", bus->name, dev->name);
    list_del(&dev->bus_list);
}
EXPORT_SYMBOL(bus_remove_device);

/**
 * bus_add_driver - add a driver into bus
 * @drv: driver
 */
state bus_add_driver(struct driver *drv)
{
    struct bus_type *bus = drv->bus;
    state retval;

    if (unlikely(!bus)) {
        pr_debug("driver %s without bus\n", drv->name);
        return -ENOERR;
    }

    pr_debug("%s add driver %s\n", bus->name, drv->name);
    list_add_prev(&bus->drivers_list, &drv->bus_list_driver);

    if (bus->autoprobe) {
        retval = driver_bind(drv);
        if (unlikely(retval))
            return retval;
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
    state retval = -ENOERR;

    if (!bus->match || !bus->probe)
        return -EINVAL;

    pr_debug("register %s bus\n", bus->name);

    spin_lock(&bus_lock);
    if (bus_find_unlock(bus->name)) {
        retval = -EALREADY;
        goto finish;
    }

    bus->autoprobe = true;
    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);
    list_add(&bus_list, &bus->list);

finish:
    spin_unlock(&bus_lock);
    return retval;
}
EXPORT_SYMBOL(bus_register);

/**
 * bus_register - remove a driver bus-type
 * @bus: bus to register
 */
void bus_unregister(struct bus_type *bus)
{
    pr_debug("unregister %s bus\n", bus->name);

    spin_lock(&bus_lock);
    list_del(&bus->list);
    spin_unlock(&bus_lock);
}
EXPORT_SYMBOL(bus_unregister);

void __init bus_init(void)
{

}
