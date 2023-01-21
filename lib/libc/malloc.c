/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <malloc.h>
#include <kmalloc.h>
#include <export.h>

void *malloc(size_t size)
{
    return kmalloc(size, GFP_KERNEL);
}
EXPORT_SYMBOL(malloc);

void *zalloc(size_t size)
{
    return kzalloc(size, GFP_KERNEL);
}
EXPORT_SYMBOL(zalloc);

void *calloc(size_t num, size_t size)
{
    return kcalloc(num, size, GFP_KERNEL);
}
EXPORT_SYMBOL(calloc);

void *realloc(void *block, size_t rsize)
{
    return krealloc(block, rsize, GFP_KERNEL);
}
EXPORT_SYMBOL(realloc);

void free(void *block)
{
    kfree(block);
}
EXPORT_SYMBOL(free);
