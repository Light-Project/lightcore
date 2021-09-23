/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVICE_H_
#define _DEVICE_DEVICE_H_

#include <mm/gfp.h>
#include <mutex.h>
#include <kobject.h>
#include <kernel/pm.h>

#include <device/bus.h>
#include <device/driver.h>

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
    struct list_head dev_res;

    struct mutex mutex;
    void *dev_data;             /* device private data */
};

#define device_for_each_res(res, dev) \
    list_for_each_entry(res, &dev->dev_res, list)

static inline void *dev_get_devdata(const struct device *dev)
{
    return dev->dev_data;
}

static inline void dev_set_devdata(struct device *dev, void *data)
{
    dev->dev_data = data;
}

void *dev_kmalloc(struct device *dev, size_t size, gfp_t gfp);
void dev_kfree(struct device *dev, void *block);
void *dev_ioremap(struct device *dev, phys_addr_t addr, size_t size);
void dev_iounmap(struct device *dev, void *block);

#define dev_kzalloc(dev, size, gfp) dev_kmalloc(dev, size, (gfp) | GFP_ZERO)
void devres_release_all(struct device *dev);

state device_bind(struct device *);
state driver_bind(struct driver *);
state device_register(struct device *);
void device_unregister(struct device *dev);

void early_device_init(void);
void device_init(void);

#endif
