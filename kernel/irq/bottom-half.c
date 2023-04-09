/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <bottom-half.h>
#include <softirq.h>
#include <crash.h>

void bh_local_enable_count(unsigned long count)
{
    BUG_ON(in_hardirq());
    raw_preempt_count_add(count);
}

void bh_local_disable_count(unsigned long count)
{
    raw_preempt_count_sub(count);
}
