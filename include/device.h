/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <mutex.h>
#include <kobject.h>
#include <pm.h>
#include <device/bus.h>
#include <device/driver.h>
#include <device/devres.h>
#include <device/devinfo.h>
#include <printk.h>

enum devres_type {
    DEVRES_KMEM,
    DEVRES_VMEM,
    DEVRES_IOMAP,
};

struct devres {
    const char *name;
    enum devres_type type;
    struct list_head list;
    void *addr;
    size_t size;
};

struct device {
    const char *name;
    struct kobject kobj;

    /* Structure description */
    struct bus_type *bus;		/* device bus */
    struct driver 	*driver;    /* device driver */
    struct device   *parent;    /* device parent */
    list_t bus_list_device;     /* list node by device bus */
    list_t driver_list_device;  /* list node by device driver */
    struct list_head devres;

    struct mutex mutex;
    void *pdata;                /* device private data */
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

state device_bind(struct device *);
state driver_bind(struct driver *);

state device_register(struct device *);
void device_unregister(struct device *dev);

void __init early_device_init(void);
void __init device_init(void);

#endif
