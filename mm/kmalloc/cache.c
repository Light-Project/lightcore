/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <kmalloc.h>
#include <export.h>

struct kcache kcache_cache;

struct kcache *kcache_create(const char *name, size_t size, size_t align)
{
    struct kcache *cache;

    cache = kcache_zalloc(&kcache_cache, GFP_KERNEL);
    if (!cache)
        return NULL;

    cache->objsize = size;
    cache->align = align;
    list_head_init(&cache->free);
    return cache;
}

void kcache_delete(struct kcache *cache)
{

}

void __init kcache_init(void)
{
    kcache_cache.objsize = sizeof(kcache_cache);
    kcache_cache.align = sizeof(kcache_cache);
    list_head_init(&kcache_cache.free);
}

EXPORT_SYMBOL(kcache_create);
EXPORT_SYMBOL(kcache_delete);
