/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "buzzer-ops"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/buzzer.h>
#include <export.h>

state buzzer_start(struct buzzer_device *bdev)
{
    struct buzzer_ops *ops;
    state retval;

    if (!bdev)
        return -EINVAL;

    ops = bdev->ops;

    if (!ops->start)
        return -EOPNOTSUPP;

    retval = ops->start(bdev);
    if (retval)
        dev_err(bdev->dev, "failed to start device retval=%d\n", retval);

    return retval;
}
EXPORT_SYMBOL(buzzer_start);

state buzzer_stop(struct buzzer_device *bdev)
{
    struct buzzer_ops *ops;
    state retval;

    if (!bdev)
        return -EINVAL;

    ops = bdev->ops;

    if (ops->stop)
        retval = ops->stop(bdev);
    else if (ops->freq_set)
        retval = ops->freq_set(bdev, 0);
    else
        return -EOPNOTSUPP;

    if (retval)
        dev_err(bdev->dev, "failed to stop device retval=%d\n", retval);

    return retval;
}
EXPORT_SYMBOL(buzzer_stop);

state buzzer_freq_get(struct buzzer_device *bdev, enum buzzer_frequency *freq)
{
    struct buzzer_ops *ops;
    state retval;

    if (!bdev || !freq)
        return -EINVAL;

    ops = bdev->ops;

    if (!ops->freq_get)
        return -EOPNOTSUPP;

    retval = ops->freq_get(bdev, freq);
    if (retval)
        dev_err(bdev->dev, "failed to get device freq retval=%d\n", retval);

    return retval;
}
EXPORT_SYMBOL(buzzer_freq_get);

state buzzer_freq_set(struct buzzer_device *bdev, enum buzzer_frequency freq)
{
    struct buzzer_ops *ops;
    state retval;

    if (!bdev)
        return -EINVAL;

    ops = bdev->ops;

    if (!ops->freq_set)
        return -EOPNOTSUPP;

    retval = ops->freq_set(bdev, freq);
    if (retval)
        dev_err(bdev->dev, "failed to set device freq retval=%d\n", retval);

    return retval;
}
EXPORT_SYMBOL(buzzer_freq_set);

static void buzzer_beep_stop(void *pdata)
{
    struct buzzer_device *bdev = pdata;
    buzzer_stop(bdev);
}

state buzzer_beep(struct buzzer_device *bdev, enum buzzer_frequency freq, int64_t msec)
{
    struct buzzer_ops *ops;
    state retval;

    if (!bdev)
        return -EINVAL;

    ops = bdev->ops;

    if (ops->beep) {
        retval = ops->beep(bdev, freq, msec);
        if (retval)
            dev_err(bdev->dev, "failed to hardware beep retval=%d\n", retval);
        return retval;
    }

    if (timer_check_pending(&bdev->beep))
        return -EBUSY;

    if ((retval = buzzer_freq_set(bdev, freq)))
        return retval;

    if ((retval = buzzer_start(bdev)))
        return retval;

    bdev->beep = TIMER_INITIALIZER(buzzer_beep_stop, bdev, ms_to_ttime(msec), 0);
    timer_pending(&bdev->beep);

    return -ENOERR;
}
EXPORT_SYMBOL(buzzer_beep);
