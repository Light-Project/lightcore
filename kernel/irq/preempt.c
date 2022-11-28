/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <preempt.h>
#include <export.h>
#include <panic.h>

DEFINE_PERCPU(unsigned long, preempt_counts);
EXPORT_SYMBOL(preempt_counts);

void preempt_count_add(unsigned long val)
{
#ifdef CONFIG_DEBUG_PREEMPT
    if (DEBUG_DATA_CHECK(preempt_count() < 0 ,
        "preempt add leaking\n"))
        return;

    DEBUG_DATA_CHECK(((preempt_count() + val) & PREEMPT_MASK)
        >= PREEMPT_MASK - 10,
        "preempt add will overflow\n");
#endif

    raw_preempt_count_add(val);
}
EXPORT_SYMBOL(preempt_count_add);

void preempt_count_sub(unsigned long val)
{
#ifdef CONFIG_DEBUG_PREEMPT
    if (DEBUG_DATA_CHECK(preempt_count() < val,
        "preempt sub count leaking\n"))
        return;

    if (DEBUG_DATA_CHECK(val < 0 && !(preempt_count() & PREEMPT_MASK),
        "preempt sub preempt leaking\n"))
        return;
#endif

    raw_preempt_count_sub(val);
}
EXPORT_SYMBOL(preempt_count_sub);
