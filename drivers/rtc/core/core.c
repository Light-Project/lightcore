/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "rtc: " fmt

#include <string.h>
#include <driver/rtc.h>
#include <printk.h>

state rtc_read_time(struct rtc_device *rtc, struct rtc_time *time)
{
    state retval;

    if (!rtc->ops)
        return -ENODEV;
    if (!rtc->ops->get_time)
        return -EINVAL;

    memset(time, 0, sizeof(*time));
    retval = rtc->ops->get_time(rtc, time);
    if (retval < 0) {
        pr_warn("read fail %d\n", retval);
    }

    return retval;
}

state rtc_set_time(struct rtc_device *rtc, struct rtc_time *time)
{
    state retval;

    if (!rtc->ops)
        return -ENODEV;
    if (!rtc->ops->set_time)
        return -EINVAL;

    retval = rtc->ops->set_time(rtc, time);
    if (retval < 0) {
        pr_warn("write fail %d\n", retval);
    }

    return retval;
}

state rtc_register(struct rtc_device *rtc_drv)
{

    return -ENOERR;
}

state rtc_unregister(struct rtc_device *rtc_drv)
{

    return -ENOERR;
}
