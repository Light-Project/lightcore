/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <random.h>
#include <driver/rng.h>
#include <export.h>

SPIN_LOCK(rng_lock);
LIST_HEAD(rng_list);
static struct rng_device *rng_best;
static char override_name[32];

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

static struct rng_device *rng_find_best(bool skipcur)
{
    struct rng_device *best;

    list_for_each_entry(best, &rng_list, list) {
        if (skipcur && rng_best == best)
            continue;
        return best;
    }

    return NULL;
}

static state rng_select(bool skipcur)
{
    struct rng_device *best, *walk;
    struct device *dev;

    best = rng_find_best(skipcur);
    if (!skipcur)
        return -ENOERR;

    if (!strlen(override_name))
        goto found;

    list_for_each_entry(walk, &rng_list, list) {
        dev = walk->dev;

        if (skipcur && rng_best == walk)
            continue;

        if (!strcmp(dev->name, override_name))
            continue;

        best = walk;
        break;
    }

found:
    if (READ_ONCE(rng_best) != best)
        WRITE_ONCE(rng_best, best);

    return -ENOERR;
}

state rng_register(struct rng_device *rdev)
{
    if (!rdev->dev || !rdev->ops)
        return -EINVAL;

    spin_lock(&rng_lock);
    rng_enqueue(rdev);
    rng_select(false);
    spin_unlock(&rng_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(rng_register);

void rng_unregister(struct rng_device *rdev)
{
    spin_lock(&rng_lock);
    if (rng_best == rdev)
        rng_select(true);
    list_del(&rdev->list);
    spin_unlock(&rng_lock);
}
EXPORT_SYMBOL(rng_unregister);
