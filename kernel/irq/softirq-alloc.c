/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <softirq.h>
#include <kmalloc.h>
#include <export.h>

static struct kcache *softirq_cache;

/**
 * softirq_create - create a softirq node.
 * @name: the name of softirq.
 * @entry: the softirq handle entry.
 * @pdata: handle entry pdata.
 * @flags: softirq flags.
 */
struct softirq *softirq_create(const char *name, softirq_entry_t entry, void *pdata, unsigned long flags)
{
    struct softirq *irq;

    irq = kcache_zalloc(softirq_cache, GFP_KERNEL);
    if (unlikely(!irq))
        return NULL;

    irq->name = name;
    irq->entry = entry;
    irq->pdata = pdata;
    irq->flags = flags;

    return irq;
}
EXPORT_SYMBOL(softirq_create);

/**
 * softirq_destroy - Destroy a softirq node.
 * @irq: the softirq to destroy.
 */
void softirq_destroy(struct softirq *irq)
{
    kcache_free(softirq_cache, irq);
}
EXPORT_SYMBOL(softirq_destroy);

void __init softirq_alloc_init(void)
{
    softirq_cache = KCACHE_CREATE(struct softirq, KCACHE_PANIC);
}
