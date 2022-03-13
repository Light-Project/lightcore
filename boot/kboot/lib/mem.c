/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <list.h>

static struct list_head sort_list;
static struct list_head free_list;
static size_t available;

struct mem_node {
    struct list_head sort;
    struct list_head free;
    size_t size;
    size_t used;
    char data[0];
};

#define data_to_mem(addr) \
    container_of(addr, struct mem_node, data)

static struct mem_node *heap_sort(struct list_head *head, size_t size)
{
    struct mem_node *tmp, *node = NULL;

    list_for_each_entry(tmp, head, free) {
        if (tmp->size >= size && (!node || node->size > tmp->size))
            node = tmp;
    }

    return node;
}

void *malloc(size_t size)
{
    struct mem_node *node, *free;

    size = align_high(size, MSIZE);
    if (size > available)
        return NULL;

    /* get the best free mem block */
    node = heap_sort(&free_list, size);
    if (!node)
        return NULL;

    if (node->size > sizeof(*free) + size) {
        free = (struct mem_node *)(node->data + size);

        /* setup the new free block */
        free->size = node->size - size - sizeof(struct mem_node);
        free->used = false;
        list_add(&free_list, &free->free);
        list_add(&node->sort, &free->sort);

        node->size = size;
        available -= sizeof(struct mem_node);
    }

    /* set used node */
    node->used = true;
    list_del(&node->free);

    /* adjust heap available size */
    available -= size;

    return node->data;
}

void free(void *mem)
{
    struct mem_node *tmp, *node;

    node = data_to_mem(mem);
    node->used = false;
    list_add(&free_list, &node->free);

    /* merge previous node */
    tmp = list_prev_entry(node, sort);
    if (!list_entry_check_head(tmp, &sort_list, sort) && !tmp->used) {
        tmp->size += sizeof(struct mem_node) + node->size;
        list_del(&node->free);
        list_del(&node->sort);
        node = tmp;
    }

    /* merge next node */
    tmp = list_next_entry(node, sort);
    if (!list_entry_check_head(tmp, &sort_list, sort) && !tmp->used) {
        node->size += sizeof(struct mem_node) + tmp->size;
        list_del(&tmp->free);
        list_del(&tmp->sort);
    }

    /* adjust heap available size */
    available += node->size;
}

void heap_init(void *addr, size_t size)
{
    struct mem_node *node = addr;

    list_head_init(&sort_list);
    list_head_init(&free_list);

    node->used = false;
    node->size = size - sizeof(*node);
    available = node->size;

    list_add(&sort_list, &node->sort);
    list_add(&free_list, &node->free);
}
