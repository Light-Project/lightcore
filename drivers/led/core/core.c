/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/led.h>
#include <export.h>

SPIN_LOCK(led_lock);
LIST_HEAD(led_list);

state led_register(struct led_device *ldev)
{
    if (!ldev->dev || !ldev->ops)
        return -EINVAL;

    spin_lock(&led_lock);
    list_add(&led_list, &ldev->list);
    led_trigger_activate(ldev);
    spin_unlock(&led_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(led_register);

void led_unregister(struct led_device *ldev)
{
    spin_lock(&led_lock);
    list_del(&ldev->list);
    led_trigger_deactivate(ldev);
    spin_unlock(&led_lock);
}
EXPORT_SYMBOL(led_unregister);
