/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/clocksource.h>
#include <printk.h>
#include <export.h>

static LIST_HEAD(clocksource_list);

static void clocksource_enqueue(struct clocksource_device *cdev)
{
    struct list_head *head = &clocksource_list;
    struct clocksource_device *walk;

    list_for_each_entry(walk, &clocksource_list, list) {
        if (walk->rating < cdev->rating)
            break;
        head = &walk->list;
    }

    list_add(head, &cdev->list);
}

static void clocksource_select(struct clocksource_device *cdev)
{

}

state clocksource_register(struct clocksource_device *cdev)
{
    if (!cdev->device || !cdev->ops)
        return -EINVAL;

    clocksource_enqueue(cdev);
    clocksource_select(cdev);

    return -ENOERR;
}
EXPORT_SYMBOL(clocksource_register);

void clocksource_config(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{
    uint64_t secs;

    secs = mask / freq;
    if (!secs)
        secs = 1;
    else if (secs > 600 && mask > UINT_MAX)
        secs = 600;

    clockevent_cloc_mult_shift(&cdev->mult, &cdev->shift, freq, NSEC_PER_SEC, secs);
}
EXPORT_SYMBOL(clocksource_config);

state clocksource_config_register(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{
    clocksource_config(cdev, freq, mask);
    return clocksource_register(cdev);
}
EXPORT_SYMBOL(clocksource_config_register);

void clocksource_unregister(struct clocksource_device *cdev)
{


}
EXPORT_SYMBOL(clocksource_unregister);

void clocksource_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clocksource_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}