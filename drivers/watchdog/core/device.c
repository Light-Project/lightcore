/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <asm/bitops.h>

state watchdog_start(struct watchdog_device *wdev)
{
    state ret;

    if (!wdev->ops->start)
        return -ENXIO;

    ret = wdev->ops->start(wdev);

    return ret;
}

state watchdog_stop(struct watchdog_device *wdev)
{
    state ret;

    if (!wdev->ops->stop)
        return -ENXIO;

    ret = wdev->ops->stop(wdev);

    return ret;
}

state watchdog_feed(struct watchdog_device *wdev)
{
    state ret;

    /* If there is no feed opt, restart the watchdog */
    if (wdev->ops->feed)
        ret = wdev->ops->feed(wdev);
    else
        ret = wdev->ops->start(wdev);

    return ret;
}

void watchdog_daemon(void *data)
{
    struct watchdog_device *wdev = data;
    watchdog_feed(wdev);
}
