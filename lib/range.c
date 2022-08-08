/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <range.h>
#include <export.h>

static __always_inline struct range_node *
range_node_alloc(struct range_head *head)
{
    return head->alloc(head->pdata);
}

static __always_inline void
range_node_free(struct range_head *head, struct range_node *node)
{
    head->free(node, head->pdata);
}

/**
 * range_insert - insert a range node.
 * @head: the range head to insert.
 * @type: the type of insert range.
 * @start: the start of insert range.
 * @size: the size of insert range.
 * @expel: forced insertion of range by expulsion.
 * @merge: merge adjacent nodes of the same type.
 */
struct range_node *range_insert(struct range_head *head, unsigned int type,
                                unsigned long start, unsigned long size,
                                bool expel, bool merge)
{
    struct range_node *block, *next, *new, *tmp, *last = NULL;
    struct list_head *prev = NULL;
    state retval;

    new = range_node_alloc(head);
    if (PTR_INVAL_ZERO(new))
        return new;

    list_for_each_entry_safe(block, next, &head->nodes, list) {
        unsigned long end, block_end;
        end = start + size - 1;
        block_end = block->start + block->size - 1;

        if (start <= block->start && block_end <= end) {
            /* completely cover */
            if (unlikely(!expel)) {
                retval = -EALREADY;
                goto failed;
            }

            range_node_free(head, block);
        } else if (start <= block->start && block->start < end) {
            /* front cover */
            if (unlikely(!expel)) {
                retval = -EALREADY;
                goto failed;
            }

            if (type == block->type) {
                size += block_end - end;
                range_node_free(head, block);
            } else {
                block->start = end + 1;
                block->size -= size;
            }
            break;
        } else if (start < block_end && block_end <= end) {
            /* back cover */
            if (unlikely(!expel)) {
                retval = -EALREADY;
                goto failed;
            }

            if (type == block->type) {
                start = block->start;
                size += start - block->start;
                prev = block->list.prev;
                range_node_free(head, block);
            } else {
                block->size = start - block->start;
                prev = &block->list;
            }
        } else if (block->start < start && end < block_end) {
            /* middle cover */
            if (unlikely(!expel)) {
                retval = -EALREADY;
                goto failed;
            }

            if (type == block->type) {
                start = block->start;
                size = block->size;
                range_node_free(head, block);
            } else {
                tmp = range_node_alloc(head);
                if (PTR_INVAL_ZERO(tmp)) {
                    retval = PTR_INVAL(tmp);
                    goto failed;
                }

                block->size = start - block->start;
                prev = &block->list;

                tmp->start = end + 1;
                tmp->size = block_end - end;
                tmp->type = block->type;
                if (head->copy)
                    head->copy(tmp, block);
                list_add(prev, &tmp->list);
            }
            break;
        }

        if (!prev && block->start < start)
            last = block;
    }

    /* init new node */
    new->start = start;
    new->size = size;
    new->type = type;

    /* insert new node */
    if (!prev) {
        if (likely(last))
            prev = &last->list;
        else  /* first node */
            prev = &head->nodes;
    }
    list_add(prev, &new->list);

    if (merge) {
        /* merge the prev same attribute node */
        if (prev != &head->nodes) {
            tmp = list_prev_entry(new, list);
            if (type == tmp->type && tmp->start + tmp->size == start) {
                new->start = tmp->start;
                new->size += tmp->size;
                range_node_free(head, tmp);
            }
        }

        /* merge the next same attribute node */
        tmp = list_next_entry_or_null(new, &head->nodes, list);
        if (tmp && type == tmp->type && start + size == tmp->start) {
            new->size += tmp->size;
            range_node_free(head, tmp);
        }
    }

    return new;

failed:
    range_node_free(head, new);
    return ERR_PTR(retval);
}
EXPORT_SYMBOL(range_insert);

/**
 * range_subtract - subtract range.
 * @head: the range head to subtract.
 * @start: start of the range to subtract.
 * @size: size of the range to subtract.
 */
state range_subtract(struct range_head *head, unsigned long start, unsigned long size)
{
    struct range_node *block, *tmp;

    list_for_each_entry(block, &head->nodes, list) {
        unsigned long end, block_end;
        end = start + size;
        block_end = block->start + block->size;

        if (start <= block->start && block_end <= end) {
            /* completely cover */
            range_node_free(head, block);
        } else if (start < block->start && block->start < end) {
            /* front cover */
            block->start = end;
            break;
        } else if (start < block_end && block_end < end) {
            /* back cover */
            block->size = block_end - start;
        } else if (block->start < start && end < block_end) {
            /* middle cover */
            tmp = range_node_alloc(head);
            if (PTR_INVAL_ZERO(tmp))
                return PTR_INVAL(tmp);

            tmp->start = end;
            tmp->size = block_end - end;
            block->start = end;
            break;
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(range_subtract);

/**
 * range_find - find the first range within a interval.
 * @head: the range head to find.
 * @type: the range type to find.
 * @align: start address alignment requirements of range.
 * @start: start address of the find range.
 * @size: size of the find range.
 */
struct range_node *range_find(struct range_head *head, unsigned int type,
                              unsigned long align, unsigned long start, unsigned long size)
{
    struct range_node *tmp, *block = NULL;
    unsigned long aligned;

    list_for_each_entry(tmp, &head->nodes, list) {
        aligned = align_high(tmp->start, align);

        if (tmp->type != type || tmp->start < start)
            continue;

        if (start + size < tmp->start)
            break;

        /* Prevent overflow caused by too large address. */
        if (aligned + size < aligned || aligned < tmp->start)
            continue;

        if (aligned + size <= tmp->start + tmp->size) {
            block = tmp;
            break;
        }
    }

    return block;
}
EXPORT_SYMBOL(range_find);
