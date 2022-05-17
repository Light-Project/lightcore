/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <mutex.h>
#include <kobject.h>
#include <device/bus.h>
#include <device/driver.h>
#include <device/devinfo.h>
#include <device/devres.h>
#include <device/power.h>
#include <printk.h>

struct fwnode;

struct device {
    const char *name;
    struct bus_type *bus;
    struct driver *driver;
    struct device *parent;
    struct fwnode *fwnode;
    void *pdata;

    struct kobject kobj;
    struct list_head bus_list_device;
    struct list_head driver_list_device;
    struct list_head devres;
    struct mutex mutex;
};

#define device_for_each_res(res, dev) \
    list_for_each_entry(res, &dev->devres, list)

static inline void *device_get_pdata(const struct device *dev)
{
    return dev->pdata;
}

static inline void device_set_pdata(struct device *dev, void *data)
{
    dev->pdata = data;
}

static inline void device_lock(struct device *dev)
{
    mutex_lock(&dev->mutex);
}

static inline void device_unlock(struct device *dev)
{
    mutex_unlock(&dev->mutex);
}

extern state device_bind(struct device *);
extern state driver_bind(struct driver *);
extern state device_register(struct device *);
extern void device_unregister(struct device *dev);

extern void __init early_device_init(void);
extern void __init device_init(void);

#endif  /* _DEVICE_H_ */
