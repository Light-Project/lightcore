/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <stddef.h>
#include <export.h>

void *kmalloc(size_t size, gfp_t gfp)
{
    void *block;

    size = align_high(size, MSIZE);

#ifdef CONFIG_MM_SLOB
    block = slob_alloc(size, gfp, MSIZE);
#elif CONFIG_MM_SLAB

#endif

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
