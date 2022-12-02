/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "devres"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <string.h>
#include <export.h>
#include <printk.h>

#ifdef CONFIG_DEBUG_DEVRES
# define devres_debug(dev, fmt, ...) dev_debug(dev, fmt, ##__VA_ARGS__)
#else
# define devres_debug(dev, fmt, ...) dev_none(dev, fmt, ##__VA_ARGS__)
#endif

static struct devres *find_unlock(struct device *dev, devres_find_t find, const void *data)
{
    struct devres *walk, *match = NULL;

    list_for_each_entry(walk, &dev->devres, list) {
        if (find(dev, walk, data)) {
            match = walk;
            break;
        }
    }

    return match;
}

struct devres *devres_find(struct device *dev, devres_find_t find, const void *data)
{
    struct devres *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    match = find_unlock(dev, find, data);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    return match;
}
EXPORT_SYMBOL(devres_find);

void devres_insert(struct device *dev, struct devres *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    list_add_prev(&dev->devres, &res->list);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    devres_debug(dev, "insert %p '%s'\n", res, res->name);
}
EXPORT_SYMBOL(devres_insert);

/**
 * devres_release - remove one resource from device.
 * @dev: device to release one.
 * @res: the release resource.
 */
void devres_remove(struct device *dev, struct devres *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    list_del(&res->list);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    devres_debug(dev, "remove %p '%s'\n", res, res->name);
}
EXPORT_SYMBOL(devres_remove);

/**
 * devres_release - release one resource from device.
 * @dev: device to release one.
 * @res: the release resource.
 */
void devres_release(struct device *dev, struct devres *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    list_del(&res->list);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    res->release(dev, res);
    devres_debug(dev, "release %p '%s'\n", res, res->name);
}
EXPORT_SYMBOL(devres_release);

struct devres *devres_find_remove(struct device *dev, devres_find_t find, const void *data)
{
    struct devres *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    match = find_unlock(dev, find, data);
    if (match)
        list_del(&match->list);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    if (match)
        devres_debug(dev, "find-remove %p '%s'\n", match, match->name);
    return match;
}
EXPORT_SYMBOL(devres_find_remove);

struct devres *devres_find_release(struct device *dev, devres_find_t find, const void *data)
{
    struct devres *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    match = find_unlock(dev, find, data);
    if (match)
        list_del(&match->list);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    if (match) {
        match->release(dev, match);
        devres_debug(dev, "find-release %p '%s'\n", match, match->name);
    }
    return match;
}
EXPORT_SYMBOL(devres_find_release);

/**
 * devres_release_all - release all resource on device.
 * @dev: device to release all.
 */
void devres_release_all(struct device *dev)
{
    struct list_head head;
    struct devres *first;
    irqflags_t irqflags;

    spin_lock_irqsave(&dev->devres_lock, &irqflags);
    if (!list_check_empty(&head)) {
        spin_unlock_irqrestore(&dev->devres_lock, &irqflags);
        return;
    }

    list_replace_init(&dev->devres, &head);
    spin_unlock_irqrestore(&dev->devres_lock, &irqflags);

    while (!list_check_empty(&head)) {
        first = list_first_entry(&head, struct devres, list);
        first->release(dev, first);
        list_del(&first->list);
        devres_debug(dev, "release-all %p '%s'\n", first, first->name);
    }
}
EXPORT_SYMBOL(devres_release_all);
