/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rtc-base"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <driver/rtc.h>
#include <export.h>

state rtc_time_get(struct rtc_device *rtc, struct rtc_time *time)
{
    state retval;

    if (!rtc->ops->time_get)
        return -EINVAL;

    memset(time, 0, sizeof(*time));
    retval = rtc->ops->time_get(rtc, time);
    if (retval < 0)
        dev_debug(rtc->dev, "time get fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_time_get);

state rtc_time_set(struct rtc_device *rtc, struct rtc_time *time)
{
    state retval;

    if (!rtc->ops->time_set)
        return -EINVAL;

    retval = rtc->ops->time_set(rtc, time);
    if (retval < 0)
        dev_debug(rtc->dev, "time set fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_time_set);

state rtc_offset_get(struct rtc_device *rtc, time_t *offset)
{
    state retval;

    if (!rtc->ops->offset_get)
        return -EOPNOTSUPP;

    retval = rtc->ops->offset_get(rtc, offset);
    if (retval < 0)
        dev_debug(rtc->dev, "offset get fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_offset_get);

state rtc_offset_set(struct rtc_device *rtc, time_t offset)
{
    state retval;

    if (!rtc->ops->offset_set)
        return -EOPNOTSUPP;

    retval = rtc->ops->offset_set(rtc, offset);
    if (retval < 0)
        dev_debug(rtc->dev, "offset set fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_offset_set);

state rtc_alarm_get(struct rtc_device *rtc, struct rtc_alarm *time)
{
    state retval;

    if (!rtc->ops->alarm_get)
        return -EOPNOTSUPP;

    memset(time, 0, sizeof(*time));
    retval = rtc->ops->alarm_get(rtc, time);
    if (retval < 0)
        dev_debug(rtc->dev, "alarm read fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_alarm_get);

state rtc_alarm_set(struct rtc_device *rtc, struct rtc_alarm *time)
{
    state retval;

    if (!rtc->ops->alarm_set)
        return -EOPNOTSUPP;

    retval = rtc->ops->alarm_set(rtc, time);
    if (retval < 0)
        dev_debug(rtc->dev, "alarm write fail %d\n", retval);

    return retval;
}
EXPORT_SYMBOL(rtc_alarm_set);
