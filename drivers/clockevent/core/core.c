/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <systicks.h>
#include <driver/clockevent.h>
#include <driver/clocksource.h>
#include <export.h>

static LIST_HEAD(clockevent_list);
static SPIN_LOCK(clockevent_lock);

static void clocksource_enqueue(struct clockevent_device *cdev)
{
    struct list_head *head = &clockevent_list;
    struct clockevent_device *walk;

    list_for_each_entry(walk, &clockevent_list, list) {
        if (walk->rating < cdev->rating)
            break;
        head = &walk->list;
    }

    list_add(head, &cdev->list);
}

// static void clocksource_select(struct clockevent_device *cdev)
// {

// }

/**
 * clockevent_register - register a clock event device
 * @cdev: clockevent device to register
 */
state clockevent_register(struct clockevent_device *cdev)
{
    if (!cdev->device || !cdev->ops)
        return -EINVAL;

    spin_lock(&clockevent_lock);
    clocksource_enqueue(cdev);
    systick_setup(cdev);
    spin_unlock(&clockevent_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(clockevent_register);

/**
 * clockevent_config - configure a clock event device
 * @cdev: clockevent device to register
 * @freq: clock frequency
 * @min_delta: minimum clock ticks oneshot mode
 * @max_delta: maximum clock ticks oneshot mode
 */
void clockevent_config(struct clockevent_device *cdev, uint64_t freq,
                       uint64_t delta_min, uint64_t delta_max)
{
    uint64_t secs;

    cdev->delta_min = delta_min;
    cdev->delta_max = delta_max;
    secs = delta_max / freq;
    if (!secs)
        secs = 1;
    else if (secs > 600 && delta_max > UINT_MAX)
        secs = 600;

    clockevent_cloc_mult_shift(&cdev->mult, &cdev->shift, NSEC_PER_SEC, freq, secs);
}
EXPORT_SYMBOL(clockevent_config);

/**
 * clockevent_config_register - configure and register a clockevent device
 * @cdev: clockevent device to register
 * @freq: clock frequency
 * @min_delta: minimum clock ticks oneshot mode
 * @max_delta: maximum clock ticks oneshot mode
 */
state clockevent_config_register(struct clockevent_device *cdev, uint64_t freq,
                                 uint64_t delta_min, uint64_t delta_max)
{
    clockevent_config(cdev, freq, delta_min, delta_max);
    return clockevent_register(cdev);
}
EXPORT_SYMBOL(clockevent_config_register);

/**
 * clockevent_unregister - unregister a clockevent device
 * @cdev: clockevent device to unregister
 */
void clockevent_unregister(struct clockevent_device *cdev)
{
}
EXPORT_SYMBOL(clockevent_unregister);

void __init clockevent_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clockevent_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }

    clocksource_init();
}
