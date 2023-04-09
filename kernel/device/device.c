/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "device"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <export.h>
#include <printk.h>

void device_shutdown(void)
{


}
EXPORT_SYMBOL(device_shutdown);

state device_register(struct device *dev)
{
    state retval;

    list_head_init(&dev->devres);

    retval = bus_add_device(dev);
    if (retval)
        return retval;

    return -ENOERR;
}
EXPORT_SYMBOL(device_register);

void device_unregister(struct device *dev)
{

}
EXPORT_SYMBOL(device_unregister);
