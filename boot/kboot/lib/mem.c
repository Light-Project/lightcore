/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <../../../lib/list.c>

static LIST_HEAD(sort_list);
static LIST_HEAD(free_list);
static size_t available;

struct mem_node {
    struct list_head sort;
    struct list_head free;
    size_t size;
    size_t used;
    char data[0];
};

static inline struct mem_node *heap_sort(list_t *head, size_t size)
{
    struct mem_node *tmp, *node = NULL;

    list_for_each_entry(tmp, head, free) {
        if (tmp->size >= size && (!node || node->size > tmp->size))
            node = tmp;
    }

    return node;
}

static inline struct mem_node *heap_get_node(list_t *head, void *addr)
{
    struct mem_node *node;

    list_for_each_entry(node, head, sort) {
        if ((void *)node->data == addr)
            return node;
    }

    return NULL;
}

void *malloc(size_t size)
{
    struct mem_node *node, *free;

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

    /* Adjust heap available size */
    available -= size;

    return node->data;
}

void free(void *mem)
{
    struct mem_node *node, *tmp;

    /* Find the freed memory node header */
    node = heap_get_node(&sort_list, mem);
    if (!node)
        return;

    node->used = false;
    list_add(&free_list, &node->free);

    /* Merge previous node */
    tmp = list_prev_entry(node, sort);
    if (!tmp->used) {
        tmp->size += sizeof(struct mem_node) + node->size;
        list_del(&node->free);
        list_del(&node->sort);
        node = tmp;
    }

    /* Merge next node */
    tmp = list_next_entry(node, sort);
    if (!tmp->used) {
        node->size += sizeof(struct mem_node) + tmp->size;
        list_del(&tmp->free);
        list_del(&tmp->sort);
    }

    /* Adjust heap available size */
    available += node->size;
}

void heap_init(void *addr, size_t size)
{
    struct mem_node *node = addr;

    list_head_init(&sort_list);
    list_head_init(&free_list);

    /* setup first free node */
    node->used = false;
    node->size = size - sizeof(*node);
    available = node->size;

    list_add(&sort_list, &node->sort);
    list_add(&free_list, &node->free);
}
