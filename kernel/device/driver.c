/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "driver"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <device/driver.h>
#include <device.h>
#include <export.h>
#include <printk.h>

/**
 * device_find - Find device through driver
 * @drv: driver to scan for the device.
 */
struct device *driver_find_device(struct driver *drv, const void *data, dfd_match_t match)
{
    struct device *dev;

    driver_for_each_device(dev, drv) {
        if (match(dev, data))
            return dev;
    }

    return NULL;
}
EXPORT_SYMBOL(driver_find_device);

/**
 * driver_find - Find device through bus
 * @bus: bus to scan for the driver.
 * @name: name of the driver.
 */
struct driver *driver_find(struct bus_type *bus, const char *name)
{
    struct driver *drv;

    bus_for_each_driver(drv, bus) {
        if (!strcmp(drv->name, name))
            return drv;
    }

    return NULL;
}
EXPORT_SYMBOL(driver_find);

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
        pr_err("driver '%s' already registered\n", drv->name);
        return -EINVAL;
    }

    list_head_init(&drv->devices_list);
    return bus_add_driver(drv);
}
EXPORT_SYMBOL(driver_register);

/**
 * driver_unregister - remove driver form bus
 * @drv: driver to register
 */
void driver_unregister(struct driver *drv)
{
    if (!drv) {
        pr_crit("illegal unregister\n");
        return;
    }

    bus_remove_driver(drv);
}
EXPORT_SYMBOL(driver_unregister);
