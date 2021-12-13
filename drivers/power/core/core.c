/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "power: " fmt

#include <driver/power.h>

static LIST_HEAD(power_list);

/**
 * power_shutdown - warm restart by power device.
 */
void power_restart(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->restart)
            pdev->ops->restart(pdev);
    }
}

/**
 * power_shutdown - cold reset by power device.
 */
void power_reset(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->reset)
            pdev->ops->reset(pdev);
    }
}

/**
 * power_shutdown - shutdown by power device.
 */
void power_shutdown(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->shutdown)
            pdev->ops->shutdown(pdev);
    }
}

/**
 * power_register - register a power device.
 * @pdev: power device to register.
 */
state power_register(struct power_device *pdev)
{
    if (!pdev->dev || !pdev->ops)
        return -EINVAL;

    list_add(&power_list, &pdev->list);
    return -ENOERR;
}

/**
 * power_unregister - unregister a power device.
 * @pdev: power device to unregister.
 */
void power_unregister(struct power_device *pdev)
{
    list_del(&pdev->list);
}
