/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <sched.h>
#include <timekeeping.h>

volatile uint64_t ticktime;
volatile unsigned long ticktime_loop;

static void ticktime_update(uint64_t ticks)
{
    uint64_t tmp;

    if ((tmp = ticktime + ticks) < ticktime)
        ticktime_loop++;

    ticktime = tmp;
}

void timer_update(unsigned long ticks)
{
    ticktime_update(ticks);
    // sched_tick();
}
