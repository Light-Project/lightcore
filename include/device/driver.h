/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICES_DRIVER_H_
#define _DEVICES_DRIVER_H_

#include <types.h>
#include <state.h>
#include <device.h>
#include <system/pm.h>

struct device;

struct driver_private {
    struct kobject kobj;
    struct klist klist_devices;
    struct klist_node knode_bus;
    struct module_kobject *mkobj;
    struct device_driver *driver;
};
#define to_driver(obj) container_of(obj, struct driver_private, kobj)

struct driver {
    const char      *name;      // driver name 
    struct bus_type *bus;       // driver bus

    struct module   *owner;     //

    state (*probe) (struct device *dev);
    state (*remove) (struct device *dev);
    void (*shutdown) (struct device *dev);
    state (*suspend) (struct device *dev, pm_message_t state);
    state (*resume) (struct device *dev);

    list_t  devices_list;
    list_t  bus_list_driver;
};

#define driver_for_each_device(device, driver)    \
        list_for_each_entry(device, &driver->devices_list, driver_list_device)

state driver_register(struct driver *drv);
void driver_unregister(struct driver *drv);

void early_device_init(void);

#endif
