/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVICE_H_
#define _DEVICE_DEVICE_H_

#include <mm/gfp.h>
#include <kernel/mutex.h>
#include <kernel/kobject.h>
#include <kernel/pm.h>

#include <device/bus.h>
#include <device/driver.h>

struct device {
    const char *name;
    struct kobject kobj;

    /* Structure description */
    struct bus_type *bus;		/* device bus */
    struct driver 	*driver;    /* device driver */
    struct device   *parent;    /* device parent */
    list_t bus_list_device;     /* List node by device bus */
    list_t driver_list_device;  /* List node by device driver */

    struct mutex mutex;
    void *dev_data;             /* device private data */
};

static inline void *dev_get_devdata(const struct device *dev)
{
    return dev->dev_data;
}

static inline void dev_set_devdata(struct device *dev, void *data)
{
    dev->dev_data = data;
}

void *dev_kmalloc(size_t size, gfp_t gfp);
void  dev_kfree(void *block);
void *dev_ioremap(resource_size_t addr, resource_size_t size);
void  dev_iounmap(resource_size_t addr, resource_size_t size);

state device_bind(struct device *);
state driver_bind(struct driver *drv);
state device_register(struct device *);

void early_device_init(void);
void device_init(void);

#endif
