/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "rtc: " fmt

#include <string.h>
#include <timekeeping.h>
#include <driver/rtc.h>

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

state rtc_register(struct rtc_device *rtc)
{
    if (!rtc->dev || !rtc->ops)
        return -EINVAL;

#ifdef CONFIG_RTC_HCTOSYS
    rtc_update_system(rtc);
#endif

    return -ENOERR;
}

void rtc_unregister(struct rtc_device *rtc)
{

}
