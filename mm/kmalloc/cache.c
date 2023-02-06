/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kcache"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
#include <string.h>
#include <export.h>
#include <printk.h>
#include <panic.h>

static LIST_HEAD(kcache_list);

struct kcache kcache_cache = {
    .name = "kcache",
    .objsize = sizeof(kcache_cache),
};

static struct kcache *kcache_find(const char *name)
{
    struct kcache *cache;

    list_for_each_entry(cache, &kcache_list, list) {
        if (!strcmp(cache->name, name))
            return cache;
    }

    return NULL;
}

/**
 * kcache_create - create a kmalloc cache.
 * @name:
 * @size:
 * @flags:
 */
struct kcache *kcache_create(const char *name, size_t size, enum kcache_flags flags)
{
    struct kcache *cache;

    if (kcache_find(name)) {
        if (flags & KCACHE_PANIC)
            panic("kcache '%s' already registered", name);
        else {
            pr_err("kcache '%s' already registered\n", name);
            return NULL;
        }
    }

    cache = kcache_zalloc(&kcache_cache, GFP_KERNEL);
    if (!cache) {
        if (flags & KCACHE_PANIC)
            panic("Failed to create '%s'", name);
        else {
            pr_err("Failed to create '%s'\n", name);
            return NULL;
        }
    }

    cache->name = name;
    cache->objsize = size;
    list_head_init(&cache->free);
    list_add(&kcache_list, &cache->list);

    return cache;
}
EXPORT_SYMBOL(kcache_create);

/**
 * kcache_delete - delete a kmalloc cache.
 * @cache: the cache to delete.
 */
void kcache_delete(struct kcache *cache)
{
    kcache_release(cache);
    list_del(&cache->list);
    kcache_free(&kcache_cache, cache);
}
EXPORT_SYMBOL(kcache_delete);

void __init kcache_init(void)
{
    list_head_init(&kcache_cache.free);
}
