/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#include <types.h>
#include <state.h>
#include <mutex.h>
#include <list.h>

struct device;
struct driver;

struct bus_type {
    const char *name;
    struct device *dev_root;

    state (*match)(struct device *, struct driver *);
    state (*probe)(struct device *);
    state (*resume)(struct device *);
    state (*remove)(struct device *);
    state (*shutdown)(struct device *);

    state (*online)(struct device *);
    state (*offline)(struct device *);

    struct mutex mutex;
    list_t devices_list;  /* devices on this bus */
    list_t drivers_list;  /* drivers on this bus */
};

#define bus_for_each_device(device, bus) \
    list_for_each_entry(device, &bus->devices_list, bus_list_device)

#define bus_for_each_driver(driver, bus) \
    list_for_each_entry(driver, &bus->drivers_list, bus_list_driver)

/* Bus highlevel API */
state bus_device_match(struct device *dev);
state bus_device_probe(struct device *dev);
state bus_add_device(struct device *drv);
void bus_remove_device(struct device *drv);

state bus_add_driver(struct driver *drv);
void bus_remove_driver(struct driver *dev);

state bus_bind_device(struct bus_type *bus, struct device *dev);
state bus_register(struct bus_type *);
void bus_unregister(struct bus_type *);

void bus_init(void);

#endif  /* _DEVICE_BUS_H_ */
