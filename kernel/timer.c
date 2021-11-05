/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <timer.h>
#include <sched.h>

volatile uint64_t ticktime;
volatile unsigned long tick_loops;

void timer_update(unsigned long ticks)
{
    ticktime += ticks;
    sched_tick();
}

void timer_tick(void)
{
    timer_update(1);
}