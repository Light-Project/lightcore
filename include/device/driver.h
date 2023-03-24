/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICES_DRIVER_H_
#define _DEVICES_DRIVER_H_

#include <list.h>
#include <spinlock.h>
#include <device/power.h>

struct device;
typedef bool (*dfd_match_t)(struct device *, const void *data);

struct driver {
    const char *name;
    struct bus_type *bus;

    spinlock_t lock;
    struct list_head bus_list_driver;
    struct list_head devices_list;

    state (*probe)(struct device *dev);
    void (*remove)(struct device *dev);

    state (*shutdown)(struct device *dev);
    state (*suspend)(struct device *dev, pm_message_t state);
    state (*resume)(struct device *dev);
};

#define driver_for_each_device(device, driver) \
    list_for_each_entry(device, &driver->devices_list, driver_list)

#define driver_for_each_device_safe(device, next, driver) \
    list_for_each_entry_safe(device, next, &driver->devices_list, driver_list)

extern struct device *driver_find_device(struct driver *drv, const void *data, dfd_match_t match);
extern struct driver *driver_find(struct bus_type *bus, const char *name);
extern state driver_register(struct driver *drv);
extern void driver_unregister(struct driver *drv);

#endif /* _DEVICES_DRIVER_H_ */
