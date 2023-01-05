/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#include <state.h>
#include <mutex.h>
#include <list.h>
#include <device/power.h>

struct device;
struct driver;

struct bus_type {
    const char *name;
    struct device *dev_root;
    struct list_head list;

    state (*match)(struct device *dev, struct driver *drv);
    state (*probe)(struct device *dev);
    state (*remove)(struct device *dev);

    state (*shutdown)(struct device *dev);
    state (*suspend)(struct device *dev, pm_message_t state);
    state (*resume)(struct device *dev);

    state (*online)(struct device *dev);
    state (*offline)(struct device *dev);

    struct mutex mutex;
    struct list_head devices_list;
    struct list_head drivers_list;

    uint32_t parent_lock:1;
    uint32_t autoprobe:1;
};

#define bus_for_each_device(device, bus) \
    list_for_each_entry(device, &bus->devices_list, bus_list)

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
