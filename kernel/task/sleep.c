/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <sched.h>
#include <timer.h>
#include <export.h>

static void timeout_wakeup(void *pdata)
{
    struct sched_task *task = pdata;
    sched_wake_up(task);
}

long schedule_timeout(ttime_t timeout)
{
    ttime_t expire;

    if (timeout == ULONG_MAX) {
        scheduler_yield();
        return timeout;
    }

    expire = ticktime + timeout;

    DEFINE_TIMER(timer, timeout_wakeup, current, timeout, 0);
    timer_pending(&timer);

    scheduler_yield();
    return expire < ticktime ? 0 : expire - ticktime;
}
EXPORT_SYMBOL(schedule_timeout);

long schedule_timeout_interruptible(ttime_t timeout)
{
    current_set_state(SCHED_TASK_INTERRUPTIBLE);
    return schedule_timeout(timeout);
}
EXPORT_SYMBOL(schedule_timeout_interruptible);

long schedule_timeout_uninterruptible(ttime_t timeout)
{
    current_set_state(SCHED_TASK_UNINTERRUPTIBLE);
    return schedule_timeout(timeout);
}
EXPORT_SYMBOL(schedule_timeout_uninterruptible);

void schedule_msleep(unsigned long msec)
{
    ttime_t timeout = ttime_to_ms(msec);
    do {
        timeout = schedule_timeout_uninterruptible(timeout);
    } while (timeout);
}
EXPORT_SYMBOL(schedule_msleep);
