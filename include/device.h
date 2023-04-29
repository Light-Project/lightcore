/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <kobject.h>
#include <respool.h>
#include <device/bus.h>
#include <device/driver.h>
#include <device/devinfo.h>
#include <device/devres.h>

struct fwnode;

struct device {
    struct mutex mutex;
    struct bus_type *bus;
    struct driver *driver;

    struct device *parent;
    struct fwnode *fwnode;

    struct list_head bus_list;
    struct list_head driver_list;
    struct respool devres;

    const char *name;
    struct kobject kobj;

    void *pdata;
    void *platform_data;
	void (*release)(struct device *dev);
};

#define device_for_each_res(res, dev) \
    list_for_each_entry(res, &dev->devres, list)

static inline const char *device_name(const struct device *dev)
{
    return kobject_get_name(&dev->kobj);
}

static inline const char *device_driver_name(const struct device *dev)
{
    /* Can change to NULL underneath us because of unbinding */
    struct driver *drv = READ_ONCE(dev->driver);
    return drv ? drv->name : NULL;
}

static inline const char *device_bus_name(const struct device *dev)
{
    /* Never change once they are set */
    return dev->bus->name;
}

static inline void *device_get_pdata(const struct device *dev)
{
    return dev->pdata;
}

static inline void device_set_pdata(struct device *dev, void *data)
{
    dev->pdata = data;
}

static inline void *device_get_platform(const struct device *dev)
{
    return dev->platform_data;
}

static inline void device_set_platform(struct device *dev, void *data)
{
    dev->platform_data = data;
}

static inline void device_lock(struct device *dev)
{
    mutex_lock(&dev->mutex);
}

static inline void device_unlock(struct device *dev)
{
    mutex_unlock(&dev->mutex);
}

static inline void device_parent_lock(struct device *dev)
{
    struct device *parent = dev->parent;
    if (parent && dev->bus->parent_lock)
        device_lock(parent);
    device_lock(dev);
}

static inline void device_parent_unlock(struct device *dev)
{
    struct device *parent = dev->parent;
    if (parent && dev->bus->parent_lock)
        device_unlock(parent);
    device_unlock(dev);
}

extern state device_bind(struct device *dev);
extern state driver_bind(struct driver *dev);
extern state device_register(struct device *dev);
extern void device_unregister(struct device *dev);

extern void __init early_device_init(void);
extern void __init device_init(void);

#endif /* _DEVICE_H_ */
