/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 Sanpe <sanpeqf@gmail.com>
 */

#include <timerqueue.h>
#include <crash.h>
#include <export.h>

static long tqueue_cmp(const struct rb_node *na, const struct rb_node *nb)
{
    struct tqueue_node *ta = container_of(na, struct tqueue_node, node);
    struct tqueue_node *tb = container_of(nb, struct tqueue_node, node);
    if (ta->timer == tb->timer) return 0;
    return ta->timer < tb->timer ? -1 : 1;
}

bool tqueue_insert(struct tqueue_head *head, struct tqueue_node *node)
{
    WARN_ON(!RB_EMPTY_NODE(&node->node));
    rb_cached_insert(&head->root, &node->node, tqueue_cmp);
    return rb_cached_first(&head->root) == &node->node;
}
EXPORT_SYMBOL(tqueue_insert);

bool tqueue_delete(struct tqueue_head *head, struct tqueue_node *node)
{
    WARN_ON(RB_EMPTY_NODE(&node->node));
    rb_cached_delete(&head->root, &node->node);
    RB_CLEAR_NODE(&node->node);
    return RB_EMPTY_ROOT_CACHED(&head->root);
}
EXPORT_SYMBOL(tqueue_delete);

struct tqueue_node *tqueue_first(struct tqueue_head *head)
{
    struct rb_node *node;

    if (unlikely(!head))
        return NULL;

    node = rb_cached_first(&head->root);
    if (!node)
        return NULL;

    return container_of(node, struct tqueue_node, node);
}
EXPORT_SYMBOL(tqueue_first);

struct tqueue_node *tqueue_last(struct tqueue_head *head)
{
    struct rb_node *node;

    if (unlikely(!head))
        return NULL;

    node = rb_cached_last(&head->root);
    if (!node)
        return NULL;

    return container_of(node, struct tqueue_node, node);
}
EXPORT_SYMBOL(tqueue_last);

struct tqueue_node *tqueue_next(struct tqueue_node *node)
{
    struct rb_node *next;

    if (unlikely(!node))
        return NULL;

    next = rb_next(&node->node);
    if (!next)
        return NULL;

    return container_of(next, struct tqueue_node, node);
}
EXPORT_SYMBOL(tqueue_next);

struct tqueue_node *tqueue_prev(struct tqueue_node *node)
{
    struct rb_node *prev;

    if (unlikely(!node))
        return NULL;

    prev = rb_prev(&node->node);
    if (!prev)
        return NULL;

    return container_of(prev, struct tqueue_node, node);
}
EXPORT_SYMBOL(tqueue_prev);
