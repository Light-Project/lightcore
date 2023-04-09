/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <minpool.h>
#include <export.h>

struct minpool_node {
    struct list_head block;
    struct list_head free;
    size_t usize;
    char data[0];
};

static __always_inline bool
gnode_get_used(struct minpool_node *node)
{
    return node->usize & BIT(0);
}

static __always_inline size_t
gnode_get_size(struct minpool_node *node)
{
    return node->usize & BIT_HIGH_MASK(1);
}

static __always_inline void
gnode_set_used(struct minpool_node *node, bool used)
{
    node->usize &= ~BIT(0);
    node->usize |= used;
}

static __always_inline void
gnode_set_size(struct minpool_node *node, size_t size)
{
    node->usize &= ~BIT_HIGH_MASK(0);
    node->usize |= size & BIT_HIGH_MASK(1);
}

static __always_inline void
gnode_set(struct minpool_node *node, size_t size, bool used)
{
    node->usize = (size & BIT_HIGH_MASK(1)) | used;
}

/**
 * minpool_find - Get first qualified node in mempool.
 * @head: Minimum mempool to get node.
 * @size: Node minimum size to get.
 */
static struct minpool_node *minpool_find(struct minpool_head *head, size_t size)
{
    struct minpool_node *node;

    list_for_each_entry(node, &head->free_list, free) {
        if (gnode_get_size(node) >= size)
            return node;
    }

    return NULL;
}

static inline struct minpool_node *minpool_check(void *block)
{
    struct minpool_node *node;

    /* Check whether it's a legal node */
    node = container_of(block, struct minpool_node, data);
    if (unlikely(!list_check_empty(&node->free)))
        return NULL;

    return node;
}

/**
 * minpool_alloc - Minimum mempool allocation.
 * @head: Minimum mempool to operate.
 * @size: Size to allocation.
 */
void *minpool_alloc(struct minpool_head *head, size_t size)
{
    struct minpool_node *node;
    size_t fsize;

    align_high_adj(size, MINPOOL_ALIGN);
    if (unlikely(size > head->avail))
        return NULL;

    /* Get the free memory block */
    node = minpool_find(head, size);
    if (unlikely(!node))
        return NULL;

    fsize = gnode_get_size(node);
    if (fsize - size >= sizeof(*node) + MINPOOL_ALIGN) {
        struct minpool_node *free;

        /* Setup the new free block */
        free = (void *)node->data + size;
        gnode_set(free, fsize - size - sizeof(*free), false);

        list_add(&head->free_list, &free->free);
        list_add(&node->block, &free->block);

        gnode_set_size(node, size);
        head->avail -= sizeof(*free);
        fsize = size;
    }

    /* Set node used */
    gnode_set_used(node, true);
    list_del_init(&node->free);

    /* Adjust heap available size */
    head->avail -= fsize;
    return node->data;
}
EXPORT_SYMBOL(minpool_alloc);

/**
 * minpool_free - Minimum mempool release.
 * @head: Minimum mempool to operate.
 * @block: Block address to free.
 */
void minpool_free(struct minpool_head *head, void *block)
{
    struct minpool_node *other, *node;
    size_t fsize;

    if (unlikely(!block))
        return;

    node = minpool_check(block);
    if (unlikely(!node))
        return;

    /* Set node freed */
    gnode_set_used(node, false);
    list_add(&head->free_list, &node->free);

    fsize = gnode_get_size(node);
    head->avail += fsize;

    /* Merge next node */
    other = list_next_entry_or_null(node, &head->block_list, block);
    if (other && !gnode_get_used(other)) {
        /* node size = this node + next node + next size */
        gnode_set_size(node, fsize + sizeof(*other) + gnode_get_size(other));
        list_del(&other->block);
        list_del(&other->free);
        head->avail += sizeof(*other);
    }

    /* Merge prev node */
    other = list_prev_entry_or_null(node, &head->block_list, block);
    if (other && !gnode_get_used(other)) {
        /* prev size = prev size + this node + this size */
        gnode_set_size(other, gnode_get_size(other) + sizeof(*node) + fsize);
        list_del(&node->block);
        list_del(&node->free);
        head->avail += sizeof(*node);
    }
}
EXPORT_SYMBOL(minpool_free);

void *minpool_realloc(struct minpool_head *head, void *block, size_t resize)
{
    struct minpool_node *node, *expand;
    size_t origin, exsize;
    void *newblk;

    if (unlikely(!resize)) {
        minpool_free(head, block);
        return NULL;
    }

    if (unlikely(!block))
        return minpool_alloc(head, resize);

    node = minpool_check(block);
    if (unlikely(!node))
        return NULL;

    align_high_adj(resize, MINPOOL_ALIGN);
    if (unlikely(resize > head->avail))
        return NULL;

    origin = gnode_get_size(node);
    if (origin >= resize)
        return block;

    expand = list_next_entry_or_null(node, &head->block_list, block);
    if (expand && !gnode_get_used(expand) && sizeof(*expand) +
        gnode_get_size(expand) >= (exsize = resize - origin)) {
        size_t fsize;

        fsize = gnode_get_size(expand);
        list_del(&expand->block);
        list_del(&expand->free);

        if (fsize - exsize < sizeof(*node) + MINPOOL_ALIGN) {
            /* Use all space of the next node */
            exsize = sizeof(*node) + fsize;
            resize = origin + exsize;
        } else {
            /* Detach free node */
            expand = (void *)expand + exsize;
            gnode_set(expand, fsize - exsize - sizeof(*node), false);
            list_add(&node->block, &expand->block);
            list_add(&head->free_list, &expand->free);
        }

        gnode_set_size(node, resize);
        head->avail -= exsize;

        return block;
    }

    newblk = minpool_alloc(head, resize);
    if (unlikely(!newblk))
        return NULL;

    memcpy(newblk, block, origin);
    minpool_free(head, block);

    return newblk;
}
EXPORT_SYMBOL(minpool_realloc);

/**
 * minpool_setup - Minimum mempool setup.
 * @head: Minimum mempool to operate.
 * @array: Mempool array address.
 * @size: Mempool array size.
 */
void minpool_setup(struct minpool_head *head, void *array, size_t size)
{
    struct minpool_node *node = array;

    gnode_set_used(node, false);
    gnode_set_size(node, size - sizeof(*node));
    head->avail = size - sizeof(*node);

    list_head_init(&head->block_list);
    list_head_init(&head->free_list);
    list_add(&head->block_list, &node->block);
    list_add(&head->free_list, &node->free);
}
EXPORT_SYMBOL(minpool_setup);
