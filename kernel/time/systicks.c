/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <systicks.h>
#include <timekeeping.h>
#include <driver/clockevent.h>

static void systick_periodic_handle(struct clockevent_device *cdev)
{
    timer_update(1);

    if (!(cdev->state == CLKEVT_STATE_ONESHOT))
        return;
}

void systick_setup(struct clockevent_device *cdev)
{
    cdev->event_handle = systick_periodic_handle;
    clockevent_switch_state(cdev, CLKEVT_STATE_PERIODIC);
}
