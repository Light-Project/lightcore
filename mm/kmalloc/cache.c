/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "kcache: " fmt

#include <kernel.h>
#include <kmalloc.h>
#include <export.h>
#include <printk.h>

struct kcache kcache_cache = {
    .objsize = sizeof(kcache_cache),
};

struct kcache *kcache_create(const char *name, size_t size, enum kcache_flags flags)
{
    struct kcache *cache;

    cache = kcache_zalloc(&kcache_cache, GFP_KERNEL);

    if (!cache) {
        if (flags & KCACHE_PANIC)
            panic("create kcache fail");
        else {
            pr_err("create fail");
            return NULL;
        }
    }

    cache->objsize = size;
    list_head_init(&cache->free);
    return cache;
}

void kcache_delete(struct kcache *cache)
{
    kcache_release(cache);
    kcache_free(&kcache_cache, cache);
}

void __init kcache_init(void)
{
    list_head_init(&kcache_cache.free);
}

EXPORT_SYMBOL(kcache_create);
EXPORT_SYMBOL(kcache_delete);
