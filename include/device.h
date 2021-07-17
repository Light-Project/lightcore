/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVICE_H_
#define _DEVICE_DEVICE_H_

#include <klist.h>
#include <system/mutex.h>
#include <system/kobject.h>
#include <system/pm.h>

#include <device/bus.h>
#include <device/driver.h>

struct device_private {
};

struct device {
    
    /* Structure description */
    struct bus_type *bus;		/* device bus */
    struct driver 	*driver;    /* device driver */
    struct device   *parent;    /* device parent */
    list_t bus_list_device;     /* List node by device bus */
    list_t driver_list_device;  /* List node by device driver */

    kobject_t       kobj;

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

state device_bind(struct device *);

#endif
