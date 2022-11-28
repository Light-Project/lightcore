/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "i2c-ops"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ticktime.h>
#include <driver/i2c-bitbang.h>
#include <export.h>

state i2c_transfer(struct i2c_host *host, unsigned int num,
                   struct i2c_transfer *trans)
{
    unsigned int count;
    ttime_t start;
    state retval = -ENOERR;

    start = ticktime;
    for (count = 0; count <= host->retries; ++count) {
        retval = host->ops->i2c_transfer(host, num, trans);

        if (retval != -EAGAIN)
            break;

        if (ttime_after(ticktime, start + host->timeout))
            break;
    }

    return retval;
}
EXPORT_SYMBOL(i2c_transfer);

state i2c_transfer_buffer(struct i2c_device *idev, uint8_t *buff,
                          unsigned int len, uint16_t flags)
{
    struct i2c_transfer trans = {
        .buf = buff,
        .len = len,
        .addr = idev->addr,
        .flags = flags | (idev->flags & I2C_M_TEN),
    };

    return i2c_transfer(idev->host, 1, &trans);
}
EXPORT_SYMBOL(i2c_transfer_buffer);
