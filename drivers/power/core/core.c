/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "power: " fmt

#include <driver/power.h>

static LIST_HEAD(power_list);

void power_restart(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->restart)
            pdev->ops->restart(pdev);
    }
}

void power_reset(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->reset)
            pdev->ops->reset(pdev);
    }
}

void power_shutdown(void)
{
    struct power_device *pdev;

    list_for_each_entry(pdev, &power_list, list) {
        if (pdev->ops->shutdown)
            pdev->ops->shutdown(pdev);
    }
}

state power_register(struct power_device *pdev)
{
    if (!pdev->ops)
        return -EINVAL;

    list_add(&power_list, &pdev->list);
    return -ENOERR;
}

void power_unregister(struct power_device *pdev)
{
    list_del(&pdev->list);
}
