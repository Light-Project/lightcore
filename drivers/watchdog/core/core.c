/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>

static LIST_HEAD(watchdog_list);

state watchdog_register(struct watchdog_device *wdev)
{
    if (!wdev->ops->feed)
        return -EINVAL;

    return -ENOERR;
}

void watchdog_unregister(struct watchdog_device *wdev)
{

}

static __init state watchdog_init(void)
{
    return -ENOERR;
}
framework_initcall(watchdog_init);
