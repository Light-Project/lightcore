/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <ncache.h>
#include <kmalloc.h>
#include <panic.h>
#include <export.h>
#include <asm/rwonce.h>

static __always_inline void
ncache_push(struct ncache *cache, void *object)
{
    cache->objects[cache->curr++] = object;
}

static __always_inline void *
ncache_pop(struct ncache *cache)
{
    return cache->objects[--cache->curr];
}

/**
 * ncache_get - obtain a nodecache object
 * @cache: cache pool to obtain node
 */
void *ncache_get(struct ncache *cache)
{
    irqflags_t irqflags;
    void *object;

    if (READ_ONCE(cache->curr)) {
        spin_lock_irqsave(&cache->lock, &irqflags);
        if (cache->curr) {
            object = ncache_pop(cache);
            spin_unlock_irqrestore(&cache->lock, &irqflags);
            return object;
        }
        spin_unlock_irqrestore(&cache->lock, &irqflags);
    }

    return cache->alloc(cache->pdata);
}
EXPORT_SYMBOL(ncache_get);

/**
 * ncache_get - release a nodecache object
 * @cache: cache pool to release node
 * @obj: node to release
 */
void ncache_put(struct ncache *cache, void *object)
{
    irqflags_t irqflags;

    BUG_ON(!object);

    if (READ_ONCE(cache->curr) < cache->num) {
        spin_lock_irqsave(&cache->lock, &irqflags);
        if (cache->curr < cache->num) {
            ncache_push(cache, object);
            spin_unlock_irqrestore(&cache->lock, &irqflags);
            return;
        }
        spin_unlock_irqrestore(&cache->lock, &irqflags);
    }

    cache->free(object, cache->pdata);
}
EXPORT_SYMBOL(ncache_put);

/**
 * ncache_charge - full of object in specific nodecache
 * @cache: the cache pool to flush
 */
state ncache_charge(struct ncache *cache)
{
    irqflags_t irqflags;
    void **object;

    while (READ_ONCE(cache->curr) < cache->num) {
        spin_lock_irqsave(&cache->lock, &irqflags);
        if (cache->curr < cache->num) {
            spin_unlock_irqrestore(&cache->lock, &irqflags);
            return -ENOERR;
        }
        object = &cache->objects[cache->curr++];
        spin_unlock_irqrestore(&cache->lock, &irqflags);
        *object = cache->alloc(cache->pdata);
        if (!*object) {
            atomic_dec(&cache->curr);
            return -ENOMEM;
        }
    }

    return -ENOERR;
}

/**
 * ncache_flush - flush all object in specific nodecache
 * @cache: the cache pool to flush
 */
void ncache_flush(struct ncache *cache)
{
    irqflags_t irqflags;
    void *object;

    while (READ_ONCE(cache->curr)) {
        spin_lock_irqsave(&cache->lock, &irqflags);
        if (!cache->curr) {
            spin_unlock_irqrestore(&cache->lock, &irqflags);
            return;
        }
        object = ncache_pop(cache);
        spin_unlock_irqrestore(&cache->lock, &irqflags);
        cache->free(object, cache->pdata);
    }
}
EXPORT_SYMBOL(ncache_flush);

/**
 * ncache_create - create a nodecache
 * @alloc: alloc callback function
 * @free: free callback function
 * @num: maximum number of nodes that can be cached
 * @flags: nodecache create flags
 * @pdata: alloc and free private parameter
 */
struct ncache *ncache_create(ncache_alloc_t alloc, ncache_free_t free, unsigned int num, enum ncache_flags flags, void *pdata)
{
    struct ncache *cache;

    cache = kzalloc(sizeof(*cache) + (sizeof(void *) * num), GFP_KERNEL);
    if (!cache) {
        if (flags & NCACHE_PANIC)
            panic("create kcache fail");
        return NULL;
    }

    cache->num = num;
    cache->alloc = alloc;
    cache->free = free;
    cache->pdata = pdata;

    if (!(flags & NCACHE_NOFULL))
        if (ncache_charge(cache))
            goto error;

    return cache;

error:
    kfree(cache);
    return NULL;
}
EXPORT_SYMBOL(ncache_create);

/**
 * ncache_destroy - destroy a specific nodecache
 * @cache: the nodecache to destroy
 */
void ncache_destroy(struct ncache *cache)
{
    ncache_flush(cache);
    kfree(cache);
}
EXPORT_SYMBOL(ncache_destroy);
