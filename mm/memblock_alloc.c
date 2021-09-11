/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/memblock.h>

static LIST_HEAD(memblk_list);

struct memblk_node {
    struct list_head list;
    size_t size;
    bool used;
};

static void *memblk_page_get(void)
{
    phys_addr_t pa;
    pa = memblock_alloc(, , CONFIG_HIGHMEM_OFFSET);
}

static struct memblk_node *heap_sort(size_t size)
{
    struct memblk_node *tmp, *node = NULL;

    list_for_each_entry(tmp, head, free_list) {
        if(tmp->size >= size)
            if((node == NULL)||(node->size > sort->size))
                node = tmp;
    }

    return node;
}

void *memblk_kmalloc(size_t size)
{


}
