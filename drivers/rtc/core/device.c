/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rtc-dev"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/rtc.h>

static void rtc_device_open(struct )
{

}

static void rtc_device_close(struct i)
{

}

static struct file_ops rtc_device_ops = {
    .open = ,
    .close = ,
    .ioctl = ,
};

state rtc_char_register(struct rtc_device *rtc)
{

    return chardev_register();
}
