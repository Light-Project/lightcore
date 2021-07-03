/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * bus driver management
 */

#include <device/bus.h>
#include <printk.h>

/**
 * bus_device_add - add a device to bus
 * @dev: device
 *  
 */
state bus_device_add(struct device *dev)
{

    return -ENOERR;
}

/**
 * bus_device_remove - remove a device to bus
 * @dev: device
 *  
 */
void bus_device_remove(struct device *dev)
{

}

/**
 * bus_driver_add - add a driver to bus
 * @drv: driver
 *  
 */
state bus_driver_add(struct driver *drv)
{
    struct bus_type *bus = drv->bus;

    list_add_prev(&bus->drivers_list, &drv->bus_list_driver);

    return -ENOERR;
}

void bus_driver_remove(struct driver *drv)
{
    if(!drv->bus)
        return;

    // klist_remove(&drv->p->kobj);
}

state bus_register(struct bus_type *bus)
{

    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);

    return -ENOERR;
}
