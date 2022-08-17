/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <skiplist.h>
#include <kmalloc.h>
#include <prandom.h>
#include <export.h>

static struct skip_node *
skipnode_find(struct skip_head *head, const void *key,
              skiplist_find_t find, unsigned int *plev)
{
    unsigned int level = head->curr;
    struct skip_node *walk, *match;
    struct list_head *list;
    long retval;

    list = &head->nodes[level - 1];
    match = list_first_entry_or_null(list, struct skip_node, list[level - 1]);

    for (; match && level--; --list) {
        retval = find(match->pdata, key);

        if (retval < 0) {
            walk = match;
            list_for_each_entry_continue(walk, list, list[level]) {
                retval = find(walk->pdata, key);
                if (retval >= 0) {
                    match = walk;
                    break;
                }
            }
        } else if (retval > 0) {
            walk = match;
            list_for_each_entry_reverse_continue(walk, list, list[level]) {
                retval = find(walk->pdata, key);
                if (retval <= 0) {
                    match = walk;
                    break;
                }
            }
        }

        if (retval == 0) {
            if (plev)
                *plev = level + 1;
            return match;
        }
    }

    return NULL;
}

static unsigned int random_level(struct skip_head *head)
{
    unsigned int level = 1;

    do {
        if (prandom_value() < UINT32_MAX >> 2)
            break;
        level++;
    } while (level < head->levels);

    return level;
}

state skiplist_insert(struct skip_head *head, void *data, skiplist_cmp_t cmp)
{
    struct skip_node *walk, *match, *node;
    struct list_head *list;
    unsigned int level;
    long tmp, retval;

    level = random_level(head);
    max_adj(head->curr, level);

    node = kmalloc(sizeof(*node) + sizeof(*node->list) * level, head->flags);
    if (unlikely(!node))
        return -ENOMEM;

    node->pdata = data;
    list = &head->nodes[level - 1];

    for (; unlikely(list_check_empty(list)); --list) {
        list_add(list, &node->list[--level]);
        if (!level)
            return -ENOERR;
    }

    match = list_first_entry(list, struct skip_node, list[level - 1]);
    for (; level--; --list) {
        retval = cmp(data, match->pdata);

        if (retval > 0) {
            walk = match;
            list_for_each_entry_continue(walk, list, list[level]) {
                tmp = cmp(data, walk->pdata);
                if (tmp < 0)
                    break;
                match = walk;
                retval = tmp;
            }
        } else if (retval < 0) {
            walk = match;
            list_for_each_entry_reverse_continue(walk, list, list[level]) {
                tmp = cmp(data, walk->pdata);
                if (tmp > 0)
                    break;
                match = walk;
                retval = tmp;
            }
        }

        if (retval >= 0)
            list_add(&match->list[level], &node->list[level]);
        else
            list_add_prev(&match->list[level], &node->list[level]);
    }

    return -ENOERR;
}
EXPORT_SYMBOL(skiplist_insert);

void skiplist_delete(struct skip_head *head, void *key, skiplist_find_t find)
{
    struct skip_node *node;
    unsigned int level;

    node = skipnode_find(head, key, find, &level);
    if (unlikely(!node))
        return;

    while (level--)
        list_del(&node->list[level]);

    kfree(node);
}
EXPORT_SYMBOL(skiplist_delete);

void *skiplist_find(struct skip_head *head, void *key, skiplist_find_t find)
{
    struct skip_node *node;
    node = skipnode_find(head, key, find, NULL);
    return node ? node->pdata : NULL;
}
EXPORT_SYMBOL(skiplist_find);

static void skiplist_release(struct skip_head *head)
{
    struct skip_node *node, *tmp;
    list_for_each_entry_safe(node, tmp, head->nodes, list[0])
        kfree(node);
}

void skiplist_reset(struct skip_head *head)
{
    unsigned int count;

    skiplist_release(head);
    for (count = 0; count < head->levels; ++count)
        list_head_init(&head->nodes[count]);

    head->count = 0;
    head->curr = 1;
}
EXPORT_SYMBOL(skiplist_reset);

struct skip_head *skiplist_create(unsigned int levels, gfp_t flags)
{
    struct skip_head *head;
    unsigned int count;

    head = kmalloc(sizeof(*head) + sizeof(*head->nodes) * levels, flags);
    if (unlikely(!head))
        return NULL;

    for (count = 0; count < levels; ++count)
        list_head_init(&head->nodes[count]);

    head->levels = levels;
    head->flags = flags;

    return head;
}
EXPORT_SYMBOL(skiplist_create);

void skiplist_destroy(struct skip_head *head)
{
    skiplist_release(head);
    kfree(head);
}
EXPORT_SYMBOL(skiplist_destroy);
