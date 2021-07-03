/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mm.h>
#include <stddef.h>
#include <export.h>

void *kmalloc(size_t size, gfp_t gfp)
{
    void *block;
#ifdef CONFIG_MM_SLOB
    block = slob_alloc(size, gfp, MSIZE);
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
