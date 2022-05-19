/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/led.h>

static state led_power_activate(struct led_device *ldev)
{
    state retval;

    retval = led_brightness_set(ldev, ldev->max_brightness);
    if (retval)
        return retval;

    return led_enable(ldev);
}

static struct led_trigger led_power_trigger = {
    .name = "power",
    .activate = led_power_activate,
};

static state led_power_init(void)
{
    return led_trigger_register(&led_power_trigger);
}
framework_initcall(led_power_init);
