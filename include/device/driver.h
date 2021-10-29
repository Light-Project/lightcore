/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICES_DRIVER_H_
#define _DEVICES_DRIVER_H_

#include <types.h>
#include <state.h>
#include <device.h>
#include <pm.h>

struct driver {
    const char *name;
    struct bus_type *bus;

    list_t  devices_list;
    list_t  bus_list_driver;
};

#define driver_for_each_device(device, driver)    \
        list_for_each_entry(device, &driver->devices_list, driver_list_device)

state driver_register(struct driver *drv);
void driver_unregister(struct driver *drv);

#endif
