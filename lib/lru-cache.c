/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <log2.h>
#include <string.h>
#include <kmalloc.h>
#include <crash.h>
#include <lru-cache.h>
#include <export.h>

static __always_inline struct hlist_head *
lrc_hash_slot(struct lrc_head *head, unsigned int tag)
{
    return head->taghash + (tag & head->nmask);
}

static __always_inline bool
lrc_unused_starving(struct lrc_head *head)
{
    return list_check_empty(&head->lru) &&
           list_check_empty(&head->freed);
}

static struct lrc_node *
lrc_find_change(struct lrc_head *head, unsigned int tag, bool change)
{
    struct lrc_node *walk;

    hlist_for_each_entry(walk, lrc_hash_slot(head, tag), hash) {
        if (walk->tag != tag)
            continue;
        if (!walk->uncommitted || change)
            return walk;
        break;
    }

    return NULL;
}

static struct lrc_node *
lrc_unused_change(struct lrc_head *head, unsigned int tag)
{
    struct lrc_node *node;

    if (!list_check_empty(&head->freed))
        node = list_first_entry(&head->freed, struct lrc_node, list);
    else if (!list_check_empty(&head->lru))
        node = list_last_entry(&head->lru, struct lrc_node, list);
    else
        return NULL;

    if (!hlist_check_unhashed(&node->hash))
        hlist_del(&node->hash);

    node->tag = tag;
    node->uncommitted = true;

    list_move(&head->changing, &node->list);
    hlist_head_add(lrc_hash_slot(head, tag), &node->hash);

    return node;
}

struct lrc_node *lrc_find(struct lrc_head *head, unsigned int tag)
{
    return lrc_find_change(head, tag, false);
}
EXPORT_SYMBOL(lrc_find);

bool lrc_check_used(struct lrc_head *head, unsigned int tag)
{
    struct lrc_node *node = lrc_find_change(head, tag, true);
    return node && node->refcnt;
}
EXPORT_SYMBOL(lrc_check_used);

struct lrc_node *lrc_obtain(struct lrc_head *head, unsigned int tag, long flags)
{
    struct lrc_node *node;

    if (unlikely(lrc_test_starving(head))) {
        head->starve++;
        return NULL;
    }

    node = lrc_find_change(head, tag, true);
    if (!node) {
        head->misses++;
        if (!(flags & LRC_CHANGE))
            return NULL;

        lrc_set_dirty(head);
        if (unlikely(lrc_unused_starving(head))) {
            lrc_set_starving(head);
            return NULL;
        }

        if (unlikely(head->pending >= head->max_pending))
            return NULL;

        node = lrc_unused_change(head, tag);
        BUG_ON(!node);

        lrc_clr_starving(head);
        head->pending++;
        head->used++;

        return node;
    }

    head->hits++;
    if (node->uncommitted) {
        if (!(flags & LRC_UNCOMMITTED))
            return NULL;
        node->refcnt++;
    } else {
        if (node->refcnt++ == 0)
            head->used++;
        list_move(&head->using, &node->list);
    }

    return node;
}
EXPORT_SYMBOL(lrc_obtain);

unsigned int lrc_put(struct lrc_head *head, struct lrc_node *node)
{
    if (!--node->refcnt) {
        lrc_clr_starving(head);
        list_move(&head->lru, &node->list);
        head->used--;
    }

    return node->refcnt;
}
EXPORT_SYMBOL(lrc_put);

state lrc_del(struct lrc_head *head, struct lrc_node *node)
{
    if (node->refcnt)
        return -EBUSY;

    node->tag = LRC_FREE_TAG;
    hlist_del_init(&node->hash);
    list_move(&head->freed, &node->list);

    return -ENOERR;
}
EXPORT_SYMBOL(lrc_del);

void lrc_set(struct lrc_head *head, struct lrc_node *node, unsigned int tag)
{
    node->tag = tag;

    if (!hlist_check_unhashed(&node->hash))
        hlist_del(&node->hash);

    if (tag == LRC_FREE_TAG) {
        hlist_node_init(&node->hash);
        list_move(&head->freed, &node->list);
    } else {
        hlist_head_add(lrc_hash_slot(head, tag), &node->hash);
        list_move(&head->lru, &node->list);
    }
}
EXPORT_SYMBOL(lrc_set);

void lrc_committed(struct lrc_head *head)
{
    struct lrc_node *node, *tmp;

    list_for_each_entry_safe(node, tmp, &head->changing, list) {
        list_move(&head->using, &node->list);
        node->uncommitted = false;
        head->changed++;
    };

    head->pending = 0;
}
EXPORT_SYMBOL(lrc_committed);

void lrc_reset(struct lrc_head *head)
{
    unsigned int number = head->nmask + 1;
    unsigned int count;
    struct lrc_node *node;

    head->flags = 0;
    head->pending = 0;
    head->used = 0;
    head->hits = 0;
    head->misses = 0;
    head->starve = 0;

    list_head_init(&head->lru);
    list_head_init(&head->using);
    list_head_init(&head->freed);
    list_head_init(&head->changing);
    memset(head->taghash, 0, sizeof(*head->taghash) * number);

    for (count = 0; count < number; ++count) {
        node = head->nodes[count];
        memset(node, 0, sizeof(*node));
        node->index = count;
        node->tag = LRC_FREE_TAG;
        list_add(&head->freed, &node->list);
    }
}
EXPORT_SYMBOL(lrc_reset);

struct lrc_head *lrc_create(unsigned int number, unsigned int maxpend,
                            lrc_alloc alloc, lrc_free free, void *pdata)
{
    struct lrc_head *head;
    struct lrc_node *node;
    unsigned int count;

    number = roundup_power_of_2(number);
    if (number < 2)
        return NULL;

    head = kzalloc(sizeof(*head), GFP_KERNEL);
    if (unlikely(!head))
        return NULL;

    head->taghash = kcalloc(number, sizeof(*head->taghash), GFP_KERNEL);
    if (unlikely(!head->taghash))
        goto free_head;

    head->nodes = kcalloc(number, sizeof(*head->nodes), GFP_KERNEL);
    if (unlikely(!head->nodes))
        goto free_htable;

    head->nmask = number - 1;
    head->max_pending = maxpend;

    list_head_init(&head->lru);
    list_head_init(&head->using);
    list_head_init(&head->freed);
    list_head_init(&head->changing);

    for (count = 0; count < number; ++count) {
        node = alloc(pdata);
        if (unlikely(!node))
            goto free_element;

        memset(node, 0, sizeof(*node));
        node->index = count;
        node->tag = LRC_FREE_TAG;
        list_add(&head->freed, &node->list);
        head->nodes[count] = node;
    }

    return head;

free_element:
    while (count--) {
        node = head->nodes[count];
        free(node, pdata);
    }
    kfree(head->nodes);
free_htable:
    kfree(head->taghash);
free_head:
    kfree(head);
    return NULL;
}
EXPORT_SYMBOL(lrc_create);

void lrc_destroy(struct lrc_head *head, lrc_free free, void *pdata)
{
    unsigned int number = head->nmask + 1;
    unsigned int count;
    struct lrc_node *node;

    for (count = 0; count < number; ++count) {
        node = head->nodes[count];
        free(node, pdata);
    }

    kfree(head->nodes);
    kfree(head->taghash);
    kfree(head);
}
EXPORT_SYMBOL(lrc_destroy);
