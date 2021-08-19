/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "kmem: " fmt

#include <mm.h>
#include <stddef.h>
#include <export.h>
#include <printk.h>

void *kmalloc_large(size_t size, gfp_t gfp)
{
    int order = to_order(size);
    struct page *page;

    page = page_alloc(order, gfp);
    if (unlikely(!page))
        return NULL;
    return page_address(page);
}
EXPORT_SYMBOL(kmalloc_large);

void *kmalloc(size_t size, gfp_t gfp)
{
    void *block;
    size = align_high(size, MSIZE);

#ifdef CONFIG_MM_SLOB
    if (size >= PAGE_SIZE)
        block = kmalloc_large(size, gfp);
    else
        block = slob_alloc(size, gfp, MSIZE);
#elif CONFIG_MM_SLAB

#endif

    if (!block)
        pr_warn("Out of memory: size 0x%x\n", size);
    return block;
}
EXPORT_SYMBOL(kmalloc);

void kfree(void *block)
{
#ifdef CONFIG_MM_SLOB
    slob_free(block);
#endif
}
EXPORT_SYMBOL(kfree);

void kmem_init(void)
{

}
EXPORT_SYMBOL(kmem_init);
