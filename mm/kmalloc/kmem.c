/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
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
 * @rsize: how many bytes of memory are required.
 * @flags: the type of memory to allocate.
 */
void *krealloc(const void *block, size_t rsize, gfp_t flags)
{
    size_t size;
    void *ret;

    if (unlikely(!rsize)) {
        kfree(block);
        return NULL;
    }

    size = ksize(block);
    if (unlikely(!size))
        return NULL;

    ret = kmalloc(rsize, flags);
    if (unlikely(!ret))
        return NULL;

    memcpy(ret, block, size);
    kfree(block);

    return ret;
}
EXPORT_SYMBOL(krealloc);

void __weak kmem_init(void)
{
    /* Nothing */
}
EXPORT_SYMBOL(kmem_init);
