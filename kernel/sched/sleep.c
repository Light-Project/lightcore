/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sleep"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <timer.h>
#include <printk.h>
#include <export.h>

static void timeout_wakeup(void *pdata)
{
    struct sched_task *task = pdata;
    sched_wake_up(task);
}

/**
 * sched_timeout - sleep until timeout.
 * @timeout: timeout value in jiffies.
 */
ttime_t sched_timeout(ttime_t timeout)
{
    ttime_t expire;

    if (timeout == TTIME_MAX) {
        sched_yield();
        goto finish;
    } else if (timeout < 0) {
        pr_err("wrong timeout value %lld\n", timeout);
        current_set_state(SCHED_TASK_RUNNING);
        goto finish;
    }

    expire = timeout + ticktime;
    DEFINE_TIMER(timer, timeout_wakeup, current, timeout, 0);

    timer_pending(&timer);
    sched_resched();
    timeout = expire - ticktime;

finish:
    return max(timeout, 0);
}
EXPORT_SYMBOL(sched_timeout);

/**
 * sched_timeout_interruptible - interruptible sleep until timeout.
 * @timeout: timeout value in jiffies.
 */
ttime_t sched_timeout_interruptible(ttime_t timeout)
{
    current_set_state(SCHED_TASK_INTERRUPTIBLE);
    return sched_timeout(timeout);
}
EXPORT_SYMBOL(sched_timeout_interruptible);

/**
 * sched_timeout_uninterruptible - uninterruptible sleep until timeout.
 * @timeout: timeout value in jiffies.
 */
ttime_t sched_timeout_uninterruptible(ttime_t timeout)
{
    current_set_state(SCHED_TASK_UNINTERRUPTIBLE);
    return sched_timeout(timeout);
}
EXPORT_SYMBOL(sched_timeout_uninterruptible);
