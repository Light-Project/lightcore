/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "device: " fmt

#include <device.h>
#include "base.h"
#include <printk.h>

state device_register(struct device *dev)
{
    if (!dev)
        return -EINVAL;

    if (bus_device_add(dev))
        return -EINVAL;

    return -ENOERR;
}
