/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <timer.h>
#include <kmalloc.h>
#include <export.h>

static struct kcache *timer_cache;

/**
 * timer_create - Create a timer node
 * @entry: the timer handle entry
 * @pdata: handle entry pdata
 * @delta: timer expires delta
 * @flags: timer flags
 */
struct timer *timer_create(timer_entry_t entry, void *pdata, ttime_t delta, enum timer_flags flags)
{
    struct timer *timer;

    timer = kcache_zalloc(timer_cache, GFP_KERNEL);
    if (unlikely(!timer))
        return NULL;

    timer->entry = entry;
    timer->pdata = pdata;
    timer->delta = delta;
    timer->flags = flags;

    return timer;
}
EXPORT_SYMBOL(timer_create);

/**
 * softirq_destroy - Destroy a timer node
 * @irq: the softirq to destroy
 */
void timer_destroy(struct timer *timer)
{
    kcache_free(timer_cache, timer);
}
EXPORT_SYMBOL(timer_destroy);

void __init timer_alloc_init(void)
{
    timer_cache = KCACHE_CREATE(struct timer, KCACHE_PANIC);
}
