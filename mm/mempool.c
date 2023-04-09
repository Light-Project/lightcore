/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "mempool"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <mempool.h>
#include <kmalloc.h>
#include <mm/page.h>
#include <export.h>
#include <printk.h>
#include <crash.h>

static struct kcache *mempool_cache;

static __always_inline void
mempool_push(struct mempool *mempool, void *object)
{
    mempool->objects[mempool->curr++] = object;
}

static __always_inline void *
mempool_pop(struct mempool *mempool)
{
    return mempool->objects[--mempool->curr];
}

/**
 * mempool_alloc - return an object to the pool
 * @pool: memory pool to allocation
 * @flags: alloc parameters
 */
void *mempool_alloc(struct mempool *pool, gfp_t flags)
{
	void *object;

    flags |= GFP_NOMEMALLOC | GFP_NORETRY | GFP_NOWARN;

    object = pool->alloc(flags, pool->data);
    if (likely(object))
        return object;

    if (likely(pool->curr))
        return mempool_pop(pool);

    return NULL;
}
EXPORT_SYMBOL(mempool_alloc);

/**
 * mempool_free - return an object to the pool
 * @pool: memory pool to release
 * @block: pool object pointer
 */
void mempool_free(struct mempool *pool, void *block)
{
    irqflags_t irqflags;

    BUG_ON(!block);

    if (unlikely(READ_ONCE(pool->curr)) < pool->demand) {
        spin_lock_irqsave(&pool->lock, &irqflags);
        if (likely(pool->curr < pool->demand)) {
            mempool_push(pool, block);
            spin_unlock_irqrestore(&pool->lock, &irqflags);
            return;
        }
        spin_unlock_irqrestore(&pool->lock, &irqflags);
    }

    pool->free(block, pool->data);
}
EXPORT_SYMBOL(mempool_free);

/**
 * mempool_resize - release all memory in a mempool
 * @pool: memory pool to release
 * @demand: number of objects resize
 * @flags: alloc parameters
 */
state mempool_resize(struct mempool *pool, unsigned int demand, gfp_t flags)
{
    irqflags_t irqflags;
    void **nobj;

    if (unlikely(!demand)) {
        mempool_release(pool);
        return -ENOERR;
    }

    spin_lock_irqsave(&pool->lock, &irqflags);
    if (demand <= pool->demand) {
        while (demand < pool->demand) {
            void *object = mempool_pop(pool);
            spin_unlock_irqrestore(&pool->lock, &irqflags);
            pool->free(object, pool->data);
            spin_lock_irqsave(&pool->lock, &irqflags);
        }
        pool->demand = demand;
        goto exit;
    }
    spin_unlock_irqrestore(&pool->lock, &irqflags);

    nobj = krealloc_array(pool->objects, demand, sizeof(void *), flags);
    if (unlikely(!nobj))
        return -ENOMEM;

    spin_lock_irqsave(&pool->lock, &irqflags);
    pool->objects = nobj;
    pool->demand = demand;

    while (pool->curr < pool->demand) {
        void *object;

        spin_unlock_irqrestore(&pool->lock, &irqflags);
        object = pool->alloc(flags, pool->data);
        if (unlikely(!object))
            return -ENOERR;

        spin_lock_irqsave(&pool->lock, &irqflags);
        if (pool->curr < pool->demand)
            mempool_push(pool, object);
        else {
            spin_unlock_irqrestore(&pool->lock, &irqflags);
            pool->free(object, pool->data);
            return -ENOERR;
        }
    }

exit:
    spin_unlock_irqrestore(&pool->lock, &irqflags);
    return -ENOERR;
}
EXPORT_SYMBOL(mempool_resize);

/**
 * mempool_create_node - release all memory in a mempool
 * @alloc: callback function for memory pool alloc
 * @free: callback function for memory pool free
 * @demand: number of objects required
 * @flags: alloc parameters
 * @data: private data of callback function
 */
struct mempool *mempool_create_node(mempool_alloc_t alloc, mempool_free_t free, unsigned int demand, gfp_t flags, void *data)
{
    struct mempool *pool;

    pool = kcache_zalloc(mempool_cache, flags);
    if (!pool)
        return NULL;

    pool->alloc = alloc;
    pool->free = free;
    pool->data = data;
    pool->demand = demand;

    pool->objects = kmalloc_array(demand, sizeof(void *), flags);
    if (!pool->objects)
        goto error_mempool;

    while (pool->curr < pool->demand) {
        void *object;

        object = pool->alloc(flags, data);
        if (unlikely(!object)) {
            mempool_release(pool);
            goto error_mempool;
        }
        mempool_push(pool, object);
    }

    return pool;

error_mempool:
    kfree(pool);
    return NULL;
}
EXPORT_SYMBOL(mempool_create_node);

/**
 * mempool_create - create a slab memory pool
 * @demand: number of objects required
 * @size: size of each object
 * @flags: alloc parameters
 */
struct mempool *mempool_create(unsigned int demand, size_t size, gfp_t flags)
{
    return mempool_create_node(mempool_kmalloc, mempool_kfree, demand, flags, (void *)size);
}
EXPORT_SYMBOL(mempool_create);

/**
 * mempool_release - release all memory in a mempool
 * @pool: memory pool to release
 */
void mempool_release(struct mempool *pool)
{
    while (pool->curr) {
        void *object = mempool_pop(pool);
        mempool_push(pool, object);
    }

    kfree(pool->objects);
    pool->objects = NULL;
}
EXPORT_SYMBOL(mempool_release);

/**
 * mempool_delete - destroy a memory pool
 * @mempool: memory pool to destroy
 */
void mempool_delete(struct mempool *pool)
{
    mempool_release(pool);
    kcache_free(mempool_cache, pool);
}
EXPORT_SYMBOL(mempool_delete);

/**
 * mempool_kmalloc/free - mempool callback for slab alloc
 */
void *mempool_kmalloc(gfp_t flags, void *size)
{
    return kmalloc((size_t)size, flags);
}
EXPORT_SYMBOL(mempool_kmalloc);

void mempool_kfree(void *block, void *data)
{
    kfree(block);
}
EXPORT_SYMBOL(mempool_kfree);

/**
 * mempool_kcache_alloc/free - mempool callback for kcache alloc
 */
void *mempool_kcache_alloc(gfp_t flags, void *cache)
{
    return kcache_alloc(cache, flags);
}
EXPORT_SYMBOL(mempool_kcache_alloc);

void mempool_kcache_free(void *block, void *cache)
{
    kcache_free(cache, block);
}
EXPORT_SYMBOL(mempool_kcache_free);

/**
 * mempool_pages_alloc/free - mempool callback for page alloc
 */
void *mempool_pages_alloc(gfp_t flags, void *order)
{
    return page_alloc((size_t)order, flags);
}
EXPORT_SYMBOL(mempool_pages_alloc);

void mempool_pages_free(void *block, void *data)
{
    page_free(block);
}
EXPORT_SYMBOL(mempool_pages_free);

void __init mempool_init(void)
{
    mempool_cache = kcache_create("mempool",
        sizeof(struct mempool), KCACHE_PANIC);
}
