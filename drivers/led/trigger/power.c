/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/led.h>

static state led_power_activate(struct led_device *ldev)
{
    led_enable(ldev);
    return led_brightness_set(ldev, ldev->max_brightness);
}

static void led_power_deactivate(struct led_device *ldev)
{
    led_brightness_set(ldev, LED_BRIGHTNESS_OFF);
    led_disable(ldev);
}

static struct led_trigger led_power_trigger = {
    .name = "power",
    .activate = led_power_activate,
    .deactivate = led_power_deactivate,
};

static state led_power_init(void)
{
    return led_trigger_register(&led_power_trigger);
}
framework_initcall(led_power_init);
