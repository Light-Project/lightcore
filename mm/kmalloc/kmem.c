/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "kmem: " fmt

#include <kernel.h>
#include <string.h>
#include <kmalloc.h>
#include <export.h>
#include <printk.h>

void *krealloc(const void *block, size_t rsize, gfp_t flags)
{
    size_t size;
    void *ret;

    if (unlikely(!rsize)) {
        kfree(block);
        return NULL;
    }

    ret = kmalloc(rsize, GFP_KERNEL);
    if (unlikely(!ret))
        return NULL;

    size = ksize(block);

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
