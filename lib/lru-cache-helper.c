/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <lru-cache.h>
#include <export.h>

static struct kcache *lrc_cache;

struct lrc_node *lrc_kmalloc(void *pdata)
{
    return kcache_alloc(lrc_cache, GFP_KERNEL);
}
EXPORT_SYMBOL(lrc_kmalloc);

void lrc_kfree(struct lrc_node *node, void *pdata)
{
    kcache_free(lrc_cache, node);
}
EXPORT_SYMBOL(lrc_kfree);

void __init lrc_init(void)
{
    lrc_cache = KCACHE_CREATE(struct lrc_node, KCACHE_PANIC);
}
