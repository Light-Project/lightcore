/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/led.h>
#include <export.h>

state led_enable(struct led_device *ldev)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->enable)
        return -EOPNOTSUPP;

    return ops->enable(ldev);
}
EXPORT_SYMBOL(led_enable);

state led_disable(struct led_device *ldev)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->disable)
        return -EOPNOTSUPP;

    return ops->disable(ldev);
}
EXPORT_SYMBOL(led_disable);

state led_brightness_get(struct led_device *ldev, enum led_brightness *bn)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->brightness_get)
        return -EOPNOTSUPP;

    return ops->brightness_get(ldev, bn);
}
EXPORT_SYMBOL(led_brightness_get);

state led_brightness_set(struct led_device *ldev, enum led_brightness bn)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->brightness_set)
        return -EOPNOTSUPP;

    return ops->brightness_set(ldev, bn);
}
EXPORT_SYMBOL(led_brightness_set);

state led_blink_get(struct led_device *ldev, unsigned int *on, unsigned int *off)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->blink_get)
        return -EOPNOTSUPP;

    return ops->blink_get(ldev, on, off);
}
EXPORT_SYMBOL(led_blink_get);

state led_blink_set(struct led_device *ldev, unsigned int on, unsigned int off)
{
    struct led_ops *ops = ldev->ops;

    if (!ops->blink_set)
        return -EOPNOTSUPP;

    return ops->blink_set(ldev, on, off);
}
EXPORT_SYMBOL(led_blink_set);
