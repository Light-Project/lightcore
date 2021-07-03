/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/include/system/driver.h
 *  driver framework
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-20      first version 
 * 
 */

#ifndef _DEVICE_DEVICE_H_
#define _DEVICE_DEVICE_H_

#include <device/base.h>

struct device_private {
};

struct device {
    kobject_t       kobj;
    struct device   *parent;

    struct bus_type *bus;		/* device bus */
    struct driver 	*driver;    /* device driver */

    struct mutex mutex;

    void *dev_data;             /* device private data */

    list_t  bus_list_device;
    list_t  driver_list_device;
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
