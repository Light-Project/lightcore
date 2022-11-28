/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "delay"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <delay.h>
#include <proc.h>
#include <sched.h>
#include <timekeeping.h>
#include <driver/clocksource.h>
#include <printk.h>
#include <export.h>

void static_call_proc_ndelay(unsigned long nsec)
{
    proc_ndelay(nsec);
}

void static_call_proc_udelay(unsigned long usec)
{
    proc_udelay(usec);
}

void static_call_proc_mdelay(unsigned long msec)
{
    proc_mdelay(msec);
}

void timer_ndelay(unsigned long nsec)
{
    ktime_t timeout = ktime_add_ns(timekeeping_get_time(), nsec);
    while (ktime_before(timekeeping_get_time(), timeout))
        cpu_relax();
}
EXPORT_SYMBOL(timer_ndelay);

void timer_udelay(unsigned long usec)
{
    ktime_t timeout = ktime_add_us(timekeeping_get_time(), usec);
    while (ktime_before(timekeeping_get_time(), timeout))
        cpu_relax();
}
EXPORT_SYMBOL(timer_udelay);

void timer_mdelay(unsigned long msec)
{
    ktime_t timeout = ktime_add_ms(timekeeping_get_time(), msec);
    while (ktime_before(timekeeping_get_time(), timeout))
        cpu_relax();
}
EXPORT_SYMBOL(timer_mdelay);

DEFINE_STATIC_CALL(ndelay, static_call_proc_ndelay);
DEFINE_STATIC_CALL(udelay, static_call_proc_udelay);
DEFINE_STATIC_CALL(mdelay, static_call_proc_mdelay);

void delay_change(struct clocksource_device *cdev)
{
    static bool delay_mode;

    if (!delay_mode && cdev->rating >= CLOCK_RATING_DESIRED) {
        static_call_update(ndelay, timer_ndelay);
        static_call_update(udelay, timer_udelay);
        static_call_update(mdelay, timer_mdelay);
        pr_info("change to timer-delay mode\n");
        delay_mode = true;
    } else if (delay_mode) {
        static_call_update(ndelay, static_call_proc_ndelay);
        static_call_update(udelay, static_call_proc_udelay);
        static_call_update(mdelay, static_call_proc_mdelay);
        pr_info("change to proc-delay mode\n");
        delay_mode = false;
    }
}

/**
 * msleep - uninterruptible sleep until timeout.
 * @msecs: timeout value in millisecond.
 */
void msleep(unsigned int msecs)
{
    ttime_t timeout = ms_to_ttime(msecs);
    do
        timeout = sched_timeout_uninterruptible(timeout);
    while (timeout);
}
EXPORT_SYMBOL(msleep);
