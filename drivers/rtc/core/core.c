/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rtc"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <timekeeping.h>
#include <driver/rtc.h>

LIST_HEAD(rtc_list);
SPIN_LOCK(rtc_lock);

#ifdef CONFIG_RTC_HCTOSYS
static void rtc_update_system(struct rtc_device *rtc)
{
    struct rtc_time time;
    struct timespec ts;
    state ret;

    ret = rtc_time_get(rtc, &time);
    if (ret) {
        dev_err(rtc->dev, "unable to update system\n");
        return;
    }

    ts = rtc_tm_to_timespec(&time);
    timekeeping_set_realtime(&ts);
}
#endif

state rtc_register(struct rtc_device *rdev)
{
    if (!rdev->dev || !rdev->ops)
        return -EINVAL;

#ifdef CONFIG_RTC_HCTOSYS
    rtc_update_system(rdev);
#endif

    spin_lock(&rtc_lock);
    list_add(&rtc_list, &rdev->list);
    spin_unlock(&rtc_lock);

    return -ENOERR;
}

void rtc_unregister(struct rtc_device *rdev)
{
    spin_lock(&rtc_lock);
    list_del(&rdev->list);
    spin_unlock(&rtc_lock);
}
