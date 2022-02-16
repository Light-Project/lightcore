/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clksrc"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <delay.h>
#include <timekeeping.h>
#include <driver/clocksource.h>
#include <printk.h>
#include <export.h>

static LIST_HEAD(clocksource_list);
static MUTEX_LOCK(clocksource_lock);
static struct clocksource_device *clocksource_curr;
static char override_name[32];

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

static struct clocksource_device *clocksource_find_best(bool skipcur, bool oneshot)
{
    struct clocksource_device *best;

    list_for_each_entry(best, &clocksource_list, list) {
        if (skipcur && clocksource_curr == best)
            continue;
        if (oneshot && !(best->flags & CLKSRC_VALID_HRES))
            continue;
        return best;
    }

    return NULL;
}

static state clocksource_select(bool skipcur)
{
    struct clocksource_device *best, *walk;
    struct device *dev;
    bool oneshot = false;
    state ret;

    best = clocksource_find_best(skipcur, oneshot);
    if (!best)
        return -ENOERR;

    if (!strlen(override_name))
        goto found;

    list_for_each_entry(walk, &clocksource_list, list) {
        dev = walk->device;

        if (skipcur && clocksource_curr == walk)
            continue;

        if (!strcmp(dev->name, override_name))
            continue;

        if (oneshot && !(walk->flags & CLKSRC_VALID_HRES)) {
            if (walk->flags & CLKSRC_UNSTABLE) {
                dev_warn(walk->device, "clocksource is unstable - can't switch\n");
                override_name[0] = '\0';
            } else
                dev_warn(walk->device, "clocksource is not oneshot compatible - deferring\n");
            break;
        }

        best = walk;
        break;
    }

found:
    if (clocksource_curr == best)
        return -ENOERR;

    /* enable the best clocksource */
    if (best->ops->enable && best->ops->enable(best)) {
        dev_crit(best->device, "Device can't enable\n");
        return -EIO;
    }

    /* notify timekeeping change device */
    if ((ret = timekeeping_change(best))) {
        if (best->ops->disable)
            best->ops->disable(best);
        return ret;
    }

    delay_change(best);

    /* if have old device, disable it */
    if (clocksource_curr && clocksource_curr->ops->disable)
        clocksource_curr->ops->disable(clocksource_curr);

    clocksource_curr = best;
    pr_info("Switched to best clocksource %s\n", best->device->name);
    return -ENOERR;
}

/**
 * clocksource_register - register a clocksource device
 * @cdev: clockevent device to register
 */
state clocksource_register(struct clocksource_device *cdev)
{
    if (!cdev->device || !cdev->ops || !cdev->ops->read)
        return -EINVAL;

    mutex_lock(&clocksource_lock);
    clocksource_enqueue(cdev);
    clocksource_select(false);
    mutex_unlock(&clocksource_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(clocksource_register);

/**
 * clocksource_unregister - unregister a clocksource device
 * @cdev: clockevent device to unregister
 */
void clocksource_unregister(struct clocksource_device *cdev)
{
    mutex_lock(&clocksource_lock);
    if (clocksource_curr == cdev)
        clocksource_select(true);
    list_del(&cdev->list);
    mutex_unlock(&clocksource_lock);
}
EXPORT_SYMBOL(clocksource_unregister);

/**
 * clocksource_config - configure a clocksource device
 * @cdev: clockevent device to register
 * @freq: clock frequency
 * @min_delta: minimum clock ticks oneshot mode
 * @max_delta: maximum clock ticks oneshot mode
 */
void clocksource_config(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{
    uint64_t secs;

    cdev->mask = mask;
    secs = mask / freq;
    if (!secs)
        secs = 1;
    else if (secs > 600 && mask > UINT_MAX)
        secs = 600;

    clock_cloc_mult_shift(&cdev->mult, &cdev->shift, freq, NSEC_PER_SEC, secs);
}
EXPORT_SYMBOL(clocksource_config);

/**
 * clocksource_config_register - configure and register a clocksource device
 * @cdev: clockevent device to register
 * @freq: clock frequency
 * @min_delta: minimum clock ticks oneshot mode
 * @max_delta: maximum clock ticks oneshot mode
 */
state clocksource_config_register(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{
    clocksource_config(cdev, freq, mask);
    return clocksource_register(cdev);
}
EXPORT_SYMBOL(clocksource_config_register);

void clocksource_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clocksource_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed\n", fn->name);
    }
}