/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <delay.h>
#include <driver/rtc.h>

static state rtc_testing(struct kshell_context *ctx, void *pdata)
{
    struct rtc_device *rdev;
    struct rtc_alarm alarm;
    state retval = -ENOERR;

    spin_lock(&rtc_lock);
    list_for_each_entry(rdev, &rtc_list, list) {
        retval = rtc_time_get(rdev, &alarm.time);
        kshell_printf(ctx, "rtc %s read time: %d\n", rdev->dev->name, retval);
        if (retval)
            break;

        kshell_printf(ctx, "  tm_nsec  = %d\n", alarm.time.tm_nsec);
        kshell_printf(ctx, "  tm_sec   = %d\n", alarm.time.tm_sec);
        kshell_printf(ctx, "  tm_min   = %d\n", alarm.time.tm_min);
        kshell_printf(ctx, "  tm_hour  = %d\n", alarm.time.tm_hour);
        kshell_printf(ctx, "  tm_mday  = %d\n", alarm.time.tm_mday);
        kshell_printf(ctx, "  tm_mon   = %d\n", alarm.time.tm_mon);
        kshell_printf(ctx, "  tm_year  = %d\n", alarm.time.tm_year);
        kshell_printf(ctx, "  tm_wday  = %d\n", alarm.time.tm_wday);
        kshell_printf(ctx, "  tm_yday  = %d\n", alarm.time.tm_yday);
        kshell_printf(ctx, "  tm_isdst = %d\n", alarm.time.tm_isdst);

        retval = rtc_alarm_get(rdev, &alarm);
        kshell_printf(ctx, "rtc %s read alarm: %d\n", rdev->dev->name, retval);
        if (retval)
            break;

        kshell_printf(ctx, "  tm_nsec  = %d\n", alarm.time.tm_nsec);
        kshell_printf(ctx, "  tm_sec   = %d\n", alarm.time.tm_sec);
        kshell_printf(ctx, "  tm_min   = %d\n", alarm.time.tm_min);
        kshell_printf(ctx, "  tm_hour  = %d\n", alarm.time.tm_hour);
        kshell_printf(ctx, "  tm_mday  = %d\n", alarm.time.tm_mday);
        kshell_printf(ctx, "  tm_mon   = %d\n", alarm.time.tm_mon);
        kshell_printf(ctx, "  tm_year  = %d\n", alarm.time.tm_year);
        kshell_printf(ctx, "  tm_wday  = %d\n", alarm.time.tm_wday);
        kshell_printf(ctx, "  tm_yday  = %d\n", alarm.time.tm_yday);
        kshell_printf(ctx, "  tm_isdst = %d\n", alarm.time.tm_isdst);
        kshell_printf(ctx, "  enable   = %d\n", alarm.enable);
        kshell_printf(ctx, "  pending  = %d\n", alarm.pending);
    }
    spin_unlock(&rtc_lock);

    return retval;
}

static struct selftest_command rtc_command = {
    .group = "driver",
    .name = "rtc",
    .desc = "rtc unit test",
    .testing = rtc_testing,
};

static state selftest_rtc_init(void)
{
    return selftest_register(&rtc_command);
}
kshell_initcall(selftest_rtc_init);
