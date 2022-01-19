/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "rtc: " fmt

#include <string.h>
#include <driver/rtc.h>

static void rtc_update_system(struct rtc_device *rtc)
{
    struct rtc_time time;
    state ret;

    ret = rtc_time_get(rtc, &time);
    if (ret) {
        dev_err(rtc->dev, "unable to update system\n");
        return;
    }

}

state rtc_register(struct rtc_device *rtc)
{
    if (!rtc->dev || !rtc->ops)


    rtc_update_system(rtc);
    return -ENOERR;
}

void rtc_unregister(struct rtc_device *rtc)
{

}
