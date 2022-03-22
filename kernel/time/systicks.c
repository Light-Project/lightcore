/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <timer.h>
#include <timekeeping.h>
#include <driver/clockevent.h>

static void systick_periodic(void)
{
    timer_update();
    timekeeping_tick();
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
