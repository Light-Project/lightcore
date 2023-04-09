/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>

LIST_HEAD(watchdog_list);
SPIN_LOCK(watchdog_lock);

state watchdog_register(struct watchdog_device *wdev)
{
    if (!wdev->dev || !wdev->ops)
        return -EINVAL;

    spin_lock(&watchdog_lock);
    list_add(&watchdog_list, &wdev->list);
    spin_unlock(&watchdog_lock);

    return -ENOERR;
}

void watchdog_unregister(struct watchdog_device *wdev)
{
    spin_lock(&watchdog_lock);
    list_del(&wdev->list);
    spin_unlock(&watchdog_lock);
}

static __init state watchdog_init(void)
{
    return -ENOERR;
}
framework_initcall(watchdog_init);
