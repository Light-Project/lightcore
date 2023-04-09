/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <range.h>
#include <overflow.h>
#include <crash.h>
#include <export.h>

static __always_inline struct range_node *
range_node_alloc(struct range_head *head)
{
    return head->alloc(head->pdata);
}

static __always_inline void
range_node_free(struct range_head *head, struct range_node *node)
{
    list_del(&node->list);
    head->free(node, head->pdata);
}

static __always_inline void
range_node_copy(struct range_head *head, struct range_node *dest,
                const struct range_node *src)
{
    if (head->copy)
        head->copy(dest, src);
}

/**
 * range_insert - insert a range node.
 * @head: the range head to insert.
 * @type: the type of insert range.
 * @start: the start of insert range.
 * @size: the size of insert range.
 * @resize: range sizing rules.
 * @merge: merge adjacent nodes of the same type.
 */
struct range_node *range_insert(struct range_head *head, unsigned int type,
                                unsigned long start, unsigned long size,
                                enum range_resize resize, bool merge)
{
    struct range_node *block, *next, *new, *tmp, *last = NULL;
    struct list_head *prev = NULL;
    unsigned long end;
    state retval;

    new = range_node_alloc(head);
    if (unlikely(!new))
        return ERR_PTR(-ENOENT);

    /* prevent size overflow */
    end = start + size - 1;
    if (unlikely(end < start))
        return ERR_PTR(-EINVAL);

    list_for_each_entry_safe(block, next, &head->nodes, list) {
        unsigned long blkend;

        blkend = block->start + block->size - 1;
        BUG_ON(blkend < block->start);

        if (start <= block->start && blkend <= end) {
            /* completely cover */
            if (unlikely(resize == RANGE_COLLISION)) {
                retval = -EADDRINUSE;
                goto failed;
            }

            if (type == block->type)
                range_node_free(head, block);
            else {
                if (resize == RANGE_DISLODGE)
                    range_node_free(head, block);
                else {
                    if (start != block->start && blkend != end) {
                        tmp = range_node_alloc(head);
                        if (unlikely(!tmp)) {
                            retval = -ENOENT;
                            goto failed;
                        }

                        tmp->start = start;
                        tmp->size = block->start - start;
                        tmp->type = type;

                        start = blkend + 1;
                        size = end - blkend;
                        prev = &block->list;

                        range_node_copy(head, tmp, new);
                        list_add_prev(prev, &tmp->list);
                    }

                    else if (start != block->start)
                        goto retraction_font;

                    else if (blkend != end)
                        goto retraction_back;

                    else {
                        retval = -ENODATA;
                        goto failed;
                    }
                }
            }
        } else if (start <= block->start && block->start <= end) {
            /* front cover */
            if (unlikely(resize == RANGE_COLLISION)) {
                retval = -EADDRINUSE;
                goto failed;
            }

            if (type == block->type) {
                size += blkend - end;
                range_node_free(head, block);
            } else {
                if (resize == RANGE_DISLODGE) {
                    block->start = end + 1;
                    block->size = blkend - block->start + 1;
                } else {
                    if (start != block->start) {
                    retraction_font:
                        size = block->start - start;
                    } else {
                        retval = -ENODATA;
                        goto failed;
                    }
                }
            }
            break;
        } else if (start <= blkend && blkend <= end) {
            /* back cover */
            if (unlikely(resize == RANGE_COLLISION)) {
                retval = -EADDRINUSE;
                goto failed;
            }

            if (type == block->type) {
                start = block->start;
                size = end - start + 1;
                prev = block->list.prev;
                range_node_free(head, block);
            } else {
                if (resize == RANGE_DISLODGE) {
                    block->size = start - block->start;
                    prev = &block->list;
                } else {
                    if (blkend != end) {
                    retraction_back:
                        start = blkend + 1;
                        size = end - start + 1;
                    } else {
                        retval = -ENODATA;
                        goto failed;
                    }
                }
            }
        } else if (block->start < start && end < blkend) {
            /* middle cover */
            if (unlikely(resize == RANGE_COLLISION)) {
                retval = -EADDRINUSE;
                goto failed;
            }

            if (type == block->type) {
                start = block->start;
                size = block->size;
                range_node_free(head, block);
            } else {
                if (resize == RANGE_DISLODGE) {
                    tmp = range_node_alloc(head);
                    if (unlikely(!tmp)) {
                        retval = -ENOENT;
                        goto failed;
                    }

                    block->size = start - block->start;
                    prev = &block->list;

                    tmp->start = end + 1;
                    tmp->size = blkend - end;
                    tmp->type = block->type;

                    range_node_copy(head, tmp, block);
                    list_add(prev, &tmp->list);
                } else {
                    retval = -ENODATA;
                    goto failed;
                }
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
        else /* first node */
            prev = &head->nodes;
    }
    list_add(prev, &new->list);

    if (merge) {
        /* merge the prev same type adjacent node */
        if (prev != &head->nodes) {
            tmp = list_prev_entry(new, list);
            if (type == tmp->type && tmp->start + tmp->size == start) {
                new->start = tmp->start;
                new->size += tmp->size;
                range_node_free(head, tmp);
            }
        }

        /* merge the next same type adjacent node */
        tmp = list_next_entry_or_null(new, &head->nodes, list);
        if (tmp && type == tmp->type && start + size == tmp->start) {
            new->size += tmp->size;
            range_node_free(head, tmp);
        }
    }

    return new;

failed:
    head->free(new, head->pdata);
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
        unsigned long end, blkend;

        end = overflow_add(start, size - 1);
        blkend = overflow_add(block->start, block->size - 1);

        if (start <= block->start && blkend <= end) {
            /* completely cover */
            range_node_free(head, block);
        } else if (start <= block->start && block->start <= end) {
            /* front cover */
            block->start = end + 1;
            block->size = blkend - block->start + 1;
            break;
        } else if (start < blkend && blkend < end) {
            /* back cover */
            block->size = start - block->start;
        } else if (block->start < start && end < blkend) {
            /* middle cover */
            tmp = range_node_alloc(head);
            if (unlikely(!tmp))
                return -ENOMEM;

            block->start = end;
            list_add(&block->list, &tmp->list);

            tmp->start = end;
            tmp->size = blkend - end;
            range_node_copy(head, tmp, block);
            break;
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(range_subtract);

void range_release(struct range_head *head)
{
    struct range_node *node, *tmp;

    list_for_each_entry_safe(node, tmp, &head->nodes, list)
        range_node_free(head, node);
}
EXPORT_SYMBOL(range_release);
