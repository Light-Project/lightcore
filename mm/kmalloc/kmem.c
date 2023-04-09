/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kmem"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kernel.h>
#include <string.h>
#include <kmalloc.h>
#include <export.h>
#include <printk.h>

/**
 * krealloc - reallocate memory.
 * @block: object to reallocate memory for.
 * @resize: how many bytes of memory are required.
 * @flags: the type of memory to allocate.
 */
void *krealloc(const void *block, size_t resize, gfp_t flags)
{
    size_t origin;
    void *newblk;

    if (unlikely(!resize)) {
        kfree(block);
        return NULL;
    }

    if (likely(!IS_INVAL(block)))
        origin = ksize(block);
    else
        origin = 0;

    if (origin >= resize)
        return (void *)block;

    newblk = kmalloc(resize, flags);
    if (likely(newblk && !IS_INVAL(block))) {
        memcpy(newblk, block, origin);
        kfree(block);
    }

    return newblk;
}
EXPORT_SYMBOL(krealloc);

void __weak kmem_init(void)
{
    /* Nothing */
}
EXPORT_SYMBOL(kmem_init);
