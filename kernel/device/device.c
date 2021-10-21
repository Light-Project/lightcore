/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "device: " fmt

#include <device.h>
#include <printk.h>

state device_register(struct device *dev)
{
    state retval;

    list_head_init(&dev->devres);

    retval = bus_device_add(dev);
    if (retval)
        return retval;

    return -ENOERR;
}

void device_unregister(struct device *dev)
{

}
