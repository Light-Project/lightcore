/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "driver: " fmt

#include <string.h>
#include <device/driver.h>
#include <device.h>
#include <printk.h>

/**
 * device_find - Find device through driver
 * @drv: driver to scan for the device.
 */
struct device *device_find(struct driver *drv, const void *data,
                bool (*match)(struct device *, const void *data))
{
    struct device *dev = NULL;

    driver_for_each_device(dev, drv)
        if(match(dev, data))
            return dev;

    return NULL;
}

/**
 * driver_find - Find device through bus
 * @bus: bus to scan for the driver.
 * @name: name of the driver.
 */
struct driver *driver_find(struct bus_type *bus, const char *name)
{
    struct driver *drv;

    bus_for_each_driver(drv, bus)
        if (!strcmp(drv->name, name))
            return drv;

    return NULL;
}

/**
 * driver_register - register driver into bus
 * @drv: driver to register
 */
state driver_register(struct driver *drv)
{
    struct driver *other;

    if (!drv->name || !drv->bus)
        return -EINVAL;

    other = driver_find(drv->bus, drv->name);
    if (other) {
        pr_err("driver %s already registered\n", drv->name);
        return -EINVAL;
    }

    list_head_init(&drv->devices_list);
    return bus_add_driver(drv);
}

/**
 * driver_unregister - remove driver form bus
 * @drv: driver to register
 */
void driver_unregister(struct driver *drv)
{
    struct device *dev;

    driver_for_each_device(dev, drv) {
    }

    bus_remove_driver(drv);
}
