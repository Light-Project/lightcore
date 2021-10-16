/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <timer.h>

volatile uint64_t ticktime;

void timer_update(unsigned long ticks)
{
    ticktime += ticks;
}

void timer_tick(void)
{
    timer_update(1);
}