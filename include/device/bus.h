/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#include <device/base.h>

typedef struct bus_type {
    const char      *name;      // The name of the bus
    const char      *dev_name;
    struct device  	*dev_root;

    state (*match)(struct device *dev, struct driver *drv);
    state (*probe)(struct device *dev);
    void (*sync_state)(struct device *dev);
    state (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
    state (*resume)(struct device *dev);
    state (*online)(struct device *dev);
    state (*offline)(struct device *dev);

    list_t devices_list;        /*  */
    list_t drivers_list;
} bus_type_t;

#define bus_for_each(bus, head)	\
        list_for_each_entry(bus, head, )

#define bus_for_each_device(device, bus) \
        list_for_each_entry(device, &bus->devices_list, bus_list_device)

#define bus_for_each_driver(driver, bus) \
        list_for_each_entry(driver, &bus->drivers_list, bus_list_driver)

state bus_register(struct bus_type *);
void bus_unregister(struct bus_type *);

state bus_driver_add(struct driver *drv);
void bus_driver_remove(struct driver *drv);

#endif
