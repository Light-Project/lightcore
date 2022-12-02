/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timer.h>
#include <crash.h>
#include <driver/led.h>

static SPIN_LOCK(heartbeat_lock);
static LIST_HEAD(heartbeat_list);
static time_t heartbeat_period = CONFIG_LED_TRIGGER_HEARTBEAT_PERIOD;

static void led_heartbeat_handle(void *unused);
static DEFINE_TIMER(led_heartbeat_timer, led_heartbeat_handle, NULL, 0, 0);

static void led_heartbeat_handle(void *unused)
{
    static int heartbeat_state;
    struct led_device *ldev;
    time_t delay;
    bool value;

    /* Acts like an actual heart beat -- thump-thump-pause ... */
	switch (heartbeat_state++ & 3) {
        case 0:
            delay = 70;
            value = true;
            break;

        case 1:
            delay = (heartbeat_period >> 2) - 70;
            value = false;
            break;

        case 2:
            delay = 70;
            value = true;
            break;

        default:
            delay = heartbeat_period - (heartbeat_period >> 2) - 70;
            value = false;
            break;
	}

    spin_lock(&heartbeat_lock);
    list_for_each_entry(ldev, &heartbeat_list, trigger_list) {
        if (value)
            led_brightness_set(ldev, ldev->max_brightness);
        else
            led_brightness_set(ldev, LED_BRIGHTNESS_OFF);
    }
    spin_unlock(&heartbeat_lock);

    led_heartbeat_timer.delta = ms_to_ttime(delay);
    BUG_ON(timer_pending(&led_heartbeat_timer));
}

static state led_heartbeat_activate(struct led_device *ldev)
{
    led_enable(ldev);
    spin_lock_bh(&heartbeat_lock);
    list_add(&heartbeat_list, &ldev->trigger_list);
    spin_unlock_bh(&heartbeat_lock);
    return -ENOERR;
}

static void led_heartbeat_deactivate(struct led_device *ldev)
{
    led_disable(ldev);
    spin_lock_bh(&heartbeat_lock);
    list_del(&ldev->trigger_list);
    spin_unlock_bh(&heartbeat_lock);
}

static struct led_trigger led_heartbeat_trigger = {
    .name = "heartbeat",
    .activate = led_heartbeat_activate,
    .deactivate = led_heartbeat_deactivate,
};

static state led_heartbeat_init(void)
{
    BUG_ON(timer_pending(&led_heartbeat_timer));
    return led_trigger_register(&led_heartbeat_trigger);
}
framework_initcall(led_heartbeat_init);
