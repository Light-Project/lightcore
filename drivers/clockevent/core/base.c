/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clkevt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/clockevent.h>
#include <export.h>

static const char *clkevt_state_name[] = {
    [CLKEVT_STATE_UNINIT] = "uninit",
    [CLKEVT_STATE_SHUTDOWN] = "shutdown",
    [CLKEVT_STATE_STOP] = "stop",
    [CLKEVT_STATE_ONESHOT] = "oneshot",
    [CLKEVT_STATE_PERIODIC] = "periodic",
};

void clockevent_cloc_mult_shift(unsigned int *mult, unsigned int *shift,
                                unsigned int from, uint64_t to, unsigned int maxsec)
{
    unsigned int sft, sftacc = 32;
    uint64_t tmp;

    /*
     * Calculate the shift factor which is limiting the conversion
     * range:
     */
    tmp = ((u64)maxsec * from) >> 32;
    while (tmp) {
        tmp >>=1;
        sftacc--;
    }

    /*
     * Find the conversion shift/mult pair which has the best
     * accuracy and fits the maxsec conversion range:
     */
    for (sft = 32; sft > 0; sft--) {
        tmp = (u64) to << sft;
        tmp += from / 2;
        tmp /= from;
        if ((tmp >> sftacc) == 0)
            break;
    }

    *mult = tmp;
    *shift = sft;
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
        ret = ops->state_oneshot(cdev);
        if (ret) {
            dev_crit(cdev->device, "device can't resume\n");
            return ret;
        }
    }

    cdev->state = CLKEVT_STATE_UNINIT;
    return -ENOERR;
}

state clockevent_program_event(struct clockevent_device *cdev, ktime_t count)
{
    struct clockevent_ops *ops = cdev->ops;
    uint64_t delta = count;
    state ret;

    if (cdev->state == CLKEVT_STATE_SHUTDOWN)
        return -EIO;

    delta = max(cdev->delta_min, delta);
    delta = min(cdev->delta_max, delta);
    delta *= cdev->mult;
    delta >>= cdev->shift;

    ret = ops->next_event(cdev, delta);
    return ret;
}
EXPORT_SYMBOL(clockevent_program_event);
