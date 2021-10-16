/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>

LIST_HEAD(watchdog_list);

state watchdog_register(struct watchdog_device *wdev)
{
    if (!wdev->ops->start &&
        !wdev->ops->feed)
        return -EINVAL;

    return -ENOERR;
}

static __init state watchdog_init(void)
{
    // kthread_create(watchdog_daemon, 0, "watchdogd");
    return -ENOERR;
}
framework_initcall(watchdog_init);
