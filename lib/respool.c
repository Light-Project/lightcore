/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "respool"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <respool.h>
#include <printk.h>
#include <export.h>

#ifdef CONFIG_DEBUG_RESPOOL
# define respool_debug(fmt, ...) pr_debug(fmt, ##__VA_ARGS__)
#else
# define respool_debug(fmt, ...) pr_none(fmt, ##__VA_ARGS__)
#endif

static struct resnode *find_unlock(struct respool *pool, respool_find_t find, const void *data)
{
    struct resnode *walk, *match = NULL;

    list_for_each_entry(walk, &pool->node, list) {
        if (find(pool, walk, data)) {
            match = walk;
            break;
        }
    }

    return match;
}

struct resnode *respool_find(struct respool *pool, respool_find_t find, const void *data)
{
    struct resnode *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    match = find_unlock(pool, find, data);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    return match;
}
EXPORT_SYMBOL(respool_find);

void respool_insert(struct respool *pool, struct resnode *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    list_add_prev(&pool->node, &res->list);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    respool_debug("%s: insert %p '%s'\n",
                  pool->name, res, res->name);
}
EXPORT_SYMBOL(respool_insert);

/**
 * respool_remove - remove one resource from respool.
 * @pool: respool to release one.
 * @res: the release resource.
 */
void respool_remove(struct respool *pool, struct resnode *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    list_del(&res->list);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    respool_debug("%s: remove %p '%s'\n",
                  pool->name, res, res->name);
}
EXPORT_SYMBOL(respool_remove);

/**
 * respool_release - release one resource from respool.
 * @pool: respool to release one.
 * @res: the release resource.
 */
void respool_release(struct respool *pool, struct resnode *res)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    list_del(&res->list);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    res->release(pool, res);
    respool_debug("%s: release %p '%s'\n",
                  pool->name, res, res->name);
}
EXPORT_SYMBOL(respool_release);

struct resnode *respool_find_remove(struct respool *pool, respool_find_t find, const void *data)
{
    struct resnode *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    match = find_unlock(pool, find, data);
    if (match)
        list_del(&match->list);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    if (match) {
        respool_debug("%s: find-remove %p '%s'\n",
                      pool->name, match, match->name);
    }

    return match;
}
EXPORT_SYMBOL(respool_find_remove);

struct resnode *respool_find_release(struct respool *pool, respool_find_t find, const void *data)
{
    struct resnode *match;
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    match = find_unlock(pool, find, data);
    if (match)
        list_del(&match->list);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    if (match) {
        match->release(pool, match);
        respool_debug("%s: find-release %p '%s'\n",
                      pool->name, match, match->name);
    }

    return match;
}
EXPORT_SYMBOL(respool_find_release);

/**
 * respool_release_all - release all resource on respool.
 * @pool: respool to release all.
 */
void respool_release_all(struct respool *pool)
{
    struct list_head head;
    struct resnode *node;
    irqflags_t irqflags;

    spin_lock_irqsave(&pool->lock, &irqflags);
    if (!list_check_empty(&head)) {
        spin_unlock_irqrestore(&pool->lock, &irqflags);
        return;
    }

    list_replace_init(&pool->node, &head);
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    list_for_each_entry(node, &head, list) {
        node->release(pool, node);
        respool_debug("%s: release-all %p '%s'\n",
                      pool->name, node, node->name);
    }
}
EXPORT_SYMBOL(respool_release_all);
