/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#include <state.h>
#include <device.h>
#include <device/driver.h>

struct device;
struct driver;

struct bus_type {
    const char *name;
    struct device *dev_root;

    state (*match)(struct device *dev, struct driver *drv);
    state (*probe)(struct device *dev);
    state (*resume)(struct device *dev);
    state (*remove)(struct device *dev);
    state (*shutdown)(struct device *dev);

    state (*online)(struct device *dev);
    state (*offline)(struct device *dev);

    struct mutex mutex;
    list_t devices_list;  /* devices on this bus */
    list_t drivers_list;  /* drivers on this bus */
};

state bus_bind_device(struct bus_type *bus, struct device *dev);
state bus_register(struct bus_type *);
void bus_unregister(struct bus_type *);

#endif  /* _DEVICE_BUS_H_ */
