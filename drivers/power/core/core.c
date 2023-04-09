/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "power-dev"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/power.h>
#include <export.h>

static LIST_HEAD(power_list);
static SPIN_LOCK(power_lock);

/**
 * power_shutdown - warm restart by power device.
 * Try warm restart with all power supplies.
 */
void power_restart(void)
{
    struct power_device *pdev;

    spin_lock(&power_lock);
    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->restart) {
            pr_emerg("trying to restart using '%s'\n", pdev->dev->name);
            pdev->ops->restart(pdev);
        }
    }
    spin_unlock(&power_lock);
}
EXPORT_SYMBOL(power_restart);

/**
 * power_shutdown - cold reset by power device.
 * Try cold reset with all power supplies.
 */
void power_reset(void)
{
    struct power_device *pdev;

    spin_lock(&power_lock);
    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->reset) {
            pr_emerg("trying to reset using '%s'\n", pdev->dev->name);
            pdev->ops->reset(pdev);
        }
    }
    spin_unlock(&power_lock);
}
EXPORT_SYMBOL(power_reset);

/**
 * power_shutdown - shutdown by power device.
 * Try shutting down with all power supplies.
 */
void power_shutdown(void)
{
    struct power_device *pdev;

    spin_lock(&power_lock);
    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->shutdown) {
            pr_emerg("trying to shutdown using '%s'\n", pdev->dev->name);
            pdev->ops->shutdown(pdev);
        }
    }
    spin_unlock(&power_lock);
}
EXPORT_SYMBOL(power_shutdown);

/**
 * power_register - register a power device.
 * @pdev: power device to register.
 */
state power_register(struct power_device *pdev)
{
    if (!pdev->ops || !pdev->dev)
        return -EINVAL;

    spin_lock(&power_lock);
    list_add(&power_list, &pdev->list);
    spin_unlock(&power_lock);
    pr_debug("register device '%s'\n", pdev->dev->name);

    return -ENOERR;
}
EXPORT_SYMBOL(power_register);

/**
 * power_unregister - unregister a power device.
 * @pdev: power device to unregister.
 */
void power_unregister(struct power_device *pdev)
{
    spin_lock(&power_lock);
    list_del(&pdev->list);
    spin_unlock(&power_lock);
    pr_debug("unregister device '%s'\n", pdev->dev->name);
}
EXPORT_SYMBOL(power_unregister);
