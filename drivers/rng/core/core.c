/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <random.h>
#include <driver/rng.h>
#include <export.h>

static LIST_HEAD(rng_list);

static void rng_enqueue(struct rng_device *rdev)
{
    struct list_head *head = &rng_list;
    struct rng_device *walk;

    list_for_each_entry(walk, &rng_list, list) {
        if (walk->rating < rdev->rating)
            break;
        head = &walk->list;
    }

    list_add(head, &rdev->list);
}

state rng_register(struct rng_device *rdev)
{
    if (!rdev->dev || !rdev->ops)
        return -EINVAL;

    rng_enqueue(rdev);
    return -ENOERR;
}

void rng_unregister(struct rng_device *rdev)
{
    list_del(&rdev->list);
}
