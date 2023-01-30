/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "watchdog-power"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/power.h>

static void wdtpower_reset(struct power_device *pdev)
{
    struct watchdog_device *wdev;

    spin_lock(&watchdog_lock);
    list_for_each_entry(wdev, &watchdog_list, list) {
        spin_unlock(&watchdog_lock);

        watchdog_feed(wdev);
        watchdog_stop(wdev);
        watchdog_timeout_set(wdev, wdev->timeout_min);
        watchdog_start(wdev);

        spin_lock(&watchdog_lock);
    }
    spin_unlock(&watchdog_lock);
}

static struct power_ops wdtpower_ops = {
    .reset = wdtpower_reset,
};

static state wdtpower_probe(struct platform_device *pdev, const void *pdata)
{
    struct power_device *power;

    power = platform_kzalloc(pdev, sizeof(*power), GFP_KERNEL);
    if (unlikely(!power))
        return -ENOMEM;

    power->dev = &pdev->dev;
    power->ops = &wdtpower_ops;
    platform_set_devdata(pdev, power);

    return power_register(power);
}

static void wdtpower_remove(struct platform_device *pdev)
{
    struct power_device *power = platform_get_devdata(pdev);
    power_unregister(power);
}

static struct platform_driver wdtpower_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = wdtpower_probe,
    .remove = wdtpower_remove,
};

static state wdtpower_init(void)
{
    struct platform_device *pdev;

    pdev = platform_unified_register(&wdtpower_driver, NULL, 0);
    if (IS_INVAL(pdev))
        return PTR_INVAL(pdev);

    return -ENOERR;
}
driver_initcall_sync(wdtpower_init);
