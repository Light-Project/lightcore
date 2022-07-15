/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <../../../lib/minpool.c>
static struct minpool_head kboot_heap;

void *heap_alloc(size_t size)
{
    return minpool_alloc(&kboot_heap, size);
}

void heap_free(void *block)
{
    minpool_free(&kboot_heap, block);
}

void heap_setup(void *array, size_t size)
{
    minpool_setup(&kboot_heap, array, size);
}
