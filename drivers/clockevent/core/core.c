/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clkevt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <panic.h>
#include <timekeeping.h>
#include <clockcloc.h>
#include <driver/clocksource.h>
#include <driver/clockevent.h>
#include <export.h>

static LIST_HEAD(clockevent_list);
static MUTEX_LOCK(clockevent_lock);
static struct clockevent_device *clockevent_curr;
static char override_name[32];

static const char *clkevt_state_name[] = {
    [CLKEVT_STATE_UNINIT]       = "uninit",
    [CLKEVT_STATE_SHUTDOWN]     = "shutdown",
    [CLKEVT_STATE_STOP]         = "stop",
    [CLKEVT_STATE_ONESHOT]      = "oneshot",
    [CLKEVT_STATE_PERIODIC]     = "periodic",
};

/**
 * clockevent_noop_handle - noop handler for shutdown clockevent
 * @cdev:
 */
static void clockevent_noop_handle(struct clockevent_device *cdev)
{
    /* Nothing */
}

static state clockevent_switch_periodic(struct clockevent_device *cdev)
{
    struct clockevent_ops *ops = cdev->ops;
    uint64_t delta;

    delta = (uint64_t)NSEC_PER_SEC / CONFIG_SYSTICK_FREQ;
    delta *= cdev->mult;
    delta >>= cdev->shift;

    return ops->state_periodic(cdev, delta);
}

state clockevent_switch_state(struct clockevent_device *cdev, enum clockevent_state cstate)
{
    struct clockevent_ops *ops = cdev->ops;
    state ret;

    /* already in state */
    if (cdev->state == cstate)
        return -ENOERR;

    if (cdev->state == CLKEVT_STATE_SHUTDOWN)
        return -EIO;

    switch (cstate) {
        case CLKEVT_STATE_SHUTDOWN:
            if (!ops->state_shutdown)
                return -EOPNOTSUPP;
            ret = ops->state_shutdown(cdev);
            break;

        case CLKEVT_STATE_STOP:
            if (!ops->stop_event)
                return -EOPNOTSUPP;
            if (cdev->state != CLKEVT_STATE_ONESHOT) {
                dev_err(cdev->device, "illegal state transition (%s -> %s)\n",
                        clkevt_state_name[cdev->state], clkevt_state_name[cstate]);
                return -EINVAL;
            }
            ret = ops->stop_event(cdev);
            cdev->event_handle = clockevent_noop_handle;
            break;

        case CLKEVT_STATE_ONESHOT:
            if (!ops->state_oneshot)
                return -EOPNOTSUPP;
            if (!(cdev->capability & CLKEVT_CAP_ONESHOT))
                return -EOPNOTSUPP;
            ret = ops->state_oneshot(cdev);
            break;

        case CLKEVT_STATE_PERIODIC:
            if (!ops->state_periodic)
                return -EOPNOTSUPP;
            if (!(cdev->capability & CLKEVT_CAP_PERIODIC))
                return -EOPNOTSUPP;
            ret = clockevent_switch_periodic(cdev);
            break;

        default:
            return -EINVAL;
    }

    if (ret) {
        dev_err(cdev->device, "aborted state transition (%s -> %s)\n",
                clkevt_state_name[cdev->state], clkevt_state_name[cstate]);
        return ret;
    } else
        dev_debug(cdev->device, "state transition (%s -> %s)\n",
                clkevt_state_name[cdev->state], clkevt_state_name[cstate]);

    cdev->state = cstate;
    return -ENOERR;
}
EXPORT_SYMBOL(clockevent_switch_state);

state clockevent_resume(struct clockevent_device *cdev, enum clockevent_state cstate)
{
    struct clockevent_ops *ops = cdev->ops;
    state ret;

    if (cdev->state != CLKEVT_STATE_SHUTDOWN) {
        dev_err(cdev->device, "illegal mode transition (%s -> %s)\n",
                clkevt_state_name[cdev->state], clkevt_state_name[cstate]);
        return -ENOERR;
    }

    if (ops->state_resume) {
        ret = ops->state_resume(cdev);
        if (ret) {
            dev_crit(cdev->device, "device can't wake up\n");
            return ret;
        }
    }

    cdev->state = CLKEVT_STATE_UNINIT;
    return -ENOERR;
}
EXPORT_SYMBOL(clockevent_resume);

/**
 * clockevent_program_event - reprogram the next event to clockevent
 * @cdev: clockevent device to program
 * @stamp: timestamp of the next event
 */
state clockevent_program_event(struct clockevent_device *cdev, ktime_t stamp)
{
    struct clockevent_ops *ops = cdev->ops;
    int64_t delta;

    if (stamp < 0)
        return -ETIME;

    /* only be invoked in oneshot mode */
    if (cdev->state != CLKEVT_STATE_ONESHOT) {
        BUG();
        return -EIO;
    }

    /* program ktime directly to the device */
    if (cdev->capability & CLKEVT_CAP_KTIME)
        return ops->next_ktime(cdev, stamp);

    /* get the time delta */
    delta = ktime_to_ns(ktime_sub(stamp, timekeeping_get_time()));
    if (delta <= 0)
        return -ETIME;

    delta = max(cdev->delta_min, delta);
    delta = min(cdev->delta_max, delta);
    delta *= cdev->mult;
    delta >>= cdev->shift;

    return ops->next_event(cdev, delta);
}
EXPORT_SYMBOL(clockevent_program_event);

static void clockevent_enqueue(struct clockevent_device *cdev)
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

static struct clockevent_device *clockevent_find_best(bool skipcur)
{
    struct clockevent_device *best;

    list_for_each_entry(best, &clockevent_list, list) {
        if (skipcur && clockevent_curr == best)
            continue;
        return best;
    }

    return NULL;
}

static state clockevent_select(bool skipcur)
{
    struct clockevent_device *best, *walk;
    struct device *dev;
    state ret;

    best = clockevent_find_best(skipcur);
    if (!best)
        return -ENOERR;

    if (!strlen(override_name))
        goto found;

    list_for_each_entry(walk, &clockevent_list, list) {
        dev = walk->device;

        if (skipcur && clockevent_curr == walk)
            continue;

        if (!strcmp(dev->name, override_name))
            continue;

        best = walk;
        break;
    }

found:
    if (clockevent_curr == best)
        return -ENOERR;

    /* notify systick change device */
    if ((ret = systick_change(best)))
        return ret;

    /* if have old device, disable it */
    if (clockevent_curr)
        clockevent_switch_state(clockevent_curr, CLKEVT_STATE_SHUTDOWN);

    clockevent_curr = best;
    pr_info("Switched to best clockevent %s\n", best->device->name);
    return -ENOERR;
}

/**
 * clockevent_register - register a clockevent device
 * @cdev: clockevent device to register
 */
state clockevent_register(struct clockevent_device *cdev)
{
    if (!cdev->device || !cdev->ops)
        return -EINVAL;

    mutex_lock(&clockevent_lock);
    clockevent_enqueue(cdev);
    clockevent_select(false);
    mutex_unlock(&clockevent_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(clockevent_register);

/**
 * clockevent_unregister - unregister a clockevent device
 * @cdev: clockevent device to unregister
 */
void clockevent_unregister(struct clockevent_device *cdev)
{
    mutex_lock(&clockevent_lock);
    if (clockevent_curr == cdev)
        clockevent_select(true);
    list_del(&cdev->list);
    mutex_unlock(&clockevent_lock);
}
EXPORT_SYMBOL(clockevent_unregister);

/**
 * clockevent_config - configure a clockevent device
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

    clock_cloc_mult_shift(&cdev->mult, &cdev->shift, NSEC_PER_SEC, freq, secs);
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

void __init clockevent_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clockevent_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed\n", fn->name);
    }

    if (list_check_empty(&clockevent_list))
        panic(pr_fmt("no any device found"));

    clocksource_init();
}
