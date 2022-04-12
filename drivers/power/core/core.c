/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "power-dev"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/power.h>

static LIST_HEAD(power_list);

/**
 * power_shutdown - warm restart by power device.
 * Try warm restart with all power supplies.
 */
void power_restart(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->restart) {
            pr_emerg("trying to restart using '%s'\n", pdev->dev->name);
            pdev->ops->restart(pdev);
        }
    }
}

/**
 * power_shutdown - cold reset by power device.
 * Try cold reset with all power supplies.
 */
void power_reset(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->reset) {
            pr_emerg("trying to reset using '%s'\n", pdev->dev->name);
            pdev->ops->reset(pdev);
        }
    }
}

/**
 * power_shutdown - shutdown by power device.
 * Try shutting down with all power supplies.
 */
void power_shutdown(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->shutdown) {
            pr_emerg("trying to shutdown using '%s'\n", pdev->dev->name);
            pdev->ops->shutdown(pdev);
        }
    }
}

/**
 * power_register - register a power device.
 * @pdev: power device to register.
 */
state power_register(struct power_device *pdev)
{
    if (!pdev->ops || !pdev->dev)
        return -EINVAL;

    pr_debug("register device '%s'\n", pdev->dev->name);
    list_add(&power_list, &pdev->list);

    return -ENOERR;
}

/**
 * power_unregister - unregister a power device.
 * @pdev: power device to unregister.
 */
void power_unregister(struct power_device *pdev)
{
    pr_debug("unregister device '%s'\n", pdev->dev->name);
    list_del(&pdev->list);
}
