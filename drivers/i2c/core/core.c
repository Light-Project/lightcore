/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "i2c: " fmt

#include <driver/i2c.h>
#include <printk.h>
#include <export.h>

SPIN_LOCK(i2c_host_lock);
LIST_HEAD(i2c_host_list);

state i2c_host_register(struct i2c_host *host)
{
    if (!host->dev || !host->ops)
        return -EINVAL;

    spin_lock(&i2c_host_lock);
    list_add(&i2c_host_list, &host->list);
    spin_unlock(&i2c_host_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(i2c_host_register);

void i2c_host_unregister(struct i2c_host *host)
{
    spin_lock(&i2c_host_lock);
    list_del(&host->list);
    spin_unlock(&i2c_host_lock);
}
EXPORT_SYMBOL(i2c_host_unregister);
