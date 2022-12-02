/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timer.h>
#include <crash.h>
#include <driver/led.h>

static SPIN_LOCK(periodic_lock);
static LIST_HEAD(periodic_list);

static void led_periodic_handle(void *unused);
static DEFINE_TIMER(led_periodic_timer, led_periodic_handle, NULL, 100, TIMER_PERIODIC);

static void led_periodic_handle(void *unused)
{
    static int periodic_state;
    struct led_device *ldev;

    spin_lock(&periodic_lock);
    list_for_each_entry(ldev, &periodic_list, trigger_list) {
        if (periodic_state & 0x01)
            led_brightness_set(ldev, ldev->max_brightness);
        else
            led_brightness_set(ldev, LED_BRIGHTNESS_OFF);
    }
    spin_unlock(&periodic_lock);

    periodic_state++;
}

static state led_periodic_activate(struct led_device *ldev)
{
    led_enable(ldev);
    spin_lock_bh(&periodic_lock);
    list_add(&periodic_list, &ldev->trigger_list);
    spin_unlock_bh(&periodic_lock);
    return -ENOERR;
}

static void led_periodic_deactivate(struct led_device *ldev)
{
    led_disable(ldev);
    spin_lock_bh(&periodic_lock);
    list_del(&ldev->trigger_list);
    spin_unlock_bh(&periodic_lock);
}

static struct led_trigger led_periodic_trigger = {
    .name = "periodic",
    .activate = led_periodic_activate,
    .deactivate = led_periodic_deactivate,
};

static state led_periodic_init(void)
{
    BUG_ON(timer_pending(&led_periodic_timer));
    return led_trigger_register(&led_periodic_trigger);
}
framework_initcall(led_periodic_init);
