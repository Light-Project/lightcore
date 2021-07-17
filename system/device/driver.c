/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <printk.h> 
#include <state.h>
#include <klist.h>
#include <device/driver.h>
#include <string.h>

/**
 * device_find - Find device through driver
 * @drv: driver to scan for the device.
 * 
 */
struct device *device_find(struct driver *drv, const void *data, 
                bool (*match)(struct device *, const void *data))
{
    struct device *dev = NULL;

    if(!drv || !match)
        return NULL;

    driver_for_each_device(dev, drv)
    {
        if(match(dev, data))
            return dev;
    }
    return NULL;
}

/**
 * device_find - Find device through bus
 * @bus: bus to scan for the driver.
 * @name: name of the driver.
 * 
 */
struct driver *driver_find(struct bus_type *bus, const char *name)
{
    struct driver *drv;
    bus_for_each_driver(drv, bus)
    {
        if(!strcmp(drv->name, name))
            return drv;
    }
    return NULL;
}

/**
 * driver_register - register driver on bus
 * 
 * 
 * @return: results of enforcement
 */

state driver_register(struct driver *drv)
{
    struct driver *other;
    
    if ((drv->bus->probe && drv->probe)     ||
        (drv->bus->remove && drv->remove)   ||
        (drv->bus->shutdown && drv->shutdown))
    pr_warn("Driver '%s' needs updating - please use "
            "bus_type methods\n", drv->name);

    other = driver_find(drv->bus, drv->name);
    if(other) {
		pr_err("Error: Driver '%s' is already registered, "
			"aborting...\n", drv->name);
    }
    
    return bus_add_driver(drv);
}

/**
 * driver_unregister - remove driver
 * 
 */
void driver_unregister(struct driver *drv)
{

    bus_remove_driver(drv);
}
