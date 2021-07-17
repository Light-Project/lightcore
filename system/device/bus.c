
/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * bus driver management
 */

#include <device/bus.h>
#include <printk.h>

state bus_probe_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;
    // state ret;

    if(!bus)
        return -EINVAL;

    mutex_lock(&bus->mutex);

    mutex_unlock(&bus->mutex);
    return -ENOERR;
}

/**
 * bus_add_device - add a device into bus
 * @dev: driver
 */
state bus_add_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    if(!bus)
        return -EINVAL;

    return -ENOERR;
}

state bus_remove_device(struct device *dev)
{
    struct bus_type *bus = dev->bus;

    if(!bus)
        return -EINVAL;
    
    return -ENOERR;
}

/**
 * bus_add_driver - add a driver into bus
 * @drv: driver
 */
state bus_add_driver(struct driver *drv)
{
    struct bus_type *bus = drv->bus;

    if(!bus)
        return -EINVAL;
    
    return -ENOERR;
}

state bus_remove_driver(struct driver *drv)
{
    struct bus_type *bus = drv->bus;

    if(!bus)
        return -EINVAL;

    return -ENOERR;
}

state bus_register(struct bus_type *bus)
{

    list_head_init(&bus->devices_list);
    list_head_init(&bus->drivers_list);

    return -ENOERR;
}

void bus_init(void)
{
    //fuck you !!!!!!shit 
     
}
