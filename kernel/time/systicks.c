/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <timer.h>
#include <timekeeping.h>
#include <driver/clockevent.h>

static void systick_periodic(void)
{
    spin_lock(&ticktime_lock);
    seqlock_write_start(&ticktime_seq);
    ticktime += 1;
    seqlock_write_end(&ticktime_seq);
    spin_unlock(&ticktime_lock);

    timekeeping_tick();
    timer_update();
    sched_tick();
}

static void systick_handle(struct clockevent_device *cdev)
{
    ktime_t next_event = cdev->next_event;

    systick_periodic();

    /* clockevent in periodic mode */
    if (cdev->state != CLKEVT_STATE_ONESHOT)
        return;

    /* clockevent in oneshot mode */
    for (;;) {
        next_event = ktime_add_ns(next_event, SYSTICK_NSEC);
        if (!clockevent_program_event(cdev, next_event))
            break;
    }
}

state systick_change(struct clockevent_device *cdev)
{
    cdev->event_handle = systick_handle;
    clockevent_switch_state(cdev, CLKEVT_STATE_PERIODIC);
    return -ENOERR;
}
