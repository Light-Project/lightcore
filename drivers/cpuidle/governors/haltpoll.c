/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/cpuidle.h>

static unsigned int __read_mostly haltpoll_limit = 200000;
static unsigned int __read_mostly haltpoll_grow_start = 50000;
static unsigned int __read_mostly haltpoll_grow = 2;
static unsigned int __read_mostly haltpoll_shrink = 2;
static bool __read_mostly haltpoll_allow_shrink = true;

static unsigned int haltpoll_select(struct cpuidle_device *cdev)
{
    struct cpuidle_driver *cdrv = cdev->driver;

    if (!cdrv->ladders)
        return 0;

    if (!cdev->poll_limit)
        return 1;

    if (!cdev->last_ladder) {
        if (cdev->poll_allow_limit)
            return 1;
        return 0;
    }

    return 0;
}

static void haltpoll_reflect(struct cpuidle_device *cdev, unsigned int index)
{
    uint64_t residency = cdev->last_residency;
    unsigned int value = cdev->poll_limit;

    if (!(cdev->last_ladder = index))
        return;

    if (residency > cdev->poll_limit && residency <= haltpoll_limit) {
        value *= haltpoll_grow;
        clamp_adj(value, haltpoll_grow_start, haltpoll_limit);
        cdev->poll_limit = value;
    } else if (residency > haltpoll_limit && haltpoll_allow_shrink) {
        if (haltpoll_shrink)
            value /= haltpoll_shrink;
        else
            value = 0;
        cdev->poll_limit = value;
    }
}

static state haltpoll_activate(struct cpuidle_device *cdev)
{
    cdev->poll_limit = 0;
    return -ENOERR;
}

static struct cpuidle_governor haltpoll_governor = {
    .name = "haltpoll",
    .rating = CPUIDLE_RATING_BASE,
    .activate = haltpoll_activate,
    .select = haltpoll_select,
    .reflect = haltpoll_reflect,
};

static state haltpoll_init(void)
{
    return cpuidle_governor_register(&haltpoll_governor);
}
postcore_initcall(haltpoll_init);
