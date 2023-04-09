/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <skiplist.h>
#include <kmalloc.h>
#include <prandom.h>
#include <export.h>

static unsigned int random_level(struct skip_head *head)
{
    unsigned int level = 1;

    while (level < head->levels) {
        if (prandom_value() > UINT32_MAX >> 2)
            break;
        level++;
    }

    return level;
}

static struct skip_node *
skipnode_find(struct skip_head *head, const void *key,
              skiplist_find_t find, unsigned int *plev)
{
    unsigned int level = head->curr;
    struct list_head *list, *end;
    struct skip_node *walk;
    long retval;

    if (unlikely(!level))
        return NULL;

    list = &head->nodes[level - 1];
    end = &head->nodes[level - 1];

    for (; level--; --list, --end) {
        list_for_each_continue(list, end) {
            walk = list_entry(list, struct skip_node, list[level]);
            retval = find(walk->pdata, key);
            if (retval >= 0) {
                end = list;
                break;
            }
        }

        if (retval == 0) {
            if (plev)
                *plev = level + 1;
            return walk;
        }

        list = end->prev;
    }

    return NULL;
}

state skiplist_insert(struct skip_head *head, void *data,
                      skiplist_cmp_t cmp, gfp_t flags)
{
    struct list_head *list, *end;
    struct skip_node *walk, *node;
    unsigned int level, count;
    long retval;

    level = random_level(head);
    head->curr = head->curr > level ? head->curr : level;

    node = kmalloc(sizeof(*node) + sizeof(*node->list) * level, flags);
    if (unlikely(!node))
        return -ENOMEM;

    node->pdata = data;
    list = &head->nodes[head->curr - 1];
    end = &head->nodes[head->curr - 1];

    for (count = head->curr; count--; --list, --end) {
        list_for_each_continue(list, end) {
            walk = list_entry(list, struct skip_node, list[count]);
            retval = cmp(walk->pdata, data);
            if (retval >= 0) {
                end = list;
                break;
            }
        }

        list = end->prev;
        if (count < level)
            list_add(list, &node->list[count]);
    }

    return -ENOERR;
}
EXPORT_SYMBOL(skiplist_insert);

void skiplist_delete(struct skip_head *head, void *key,
                     skiplist_find_t find)
{
    struct skip_node *node;
    unsigned int level;

    node = skipnode_find(head, key, find, &level);
    if (unlikely(!node))
        return;

    while (level--) {
        list_del(&node->list[level]);
        if (list_check_empty(&head->nodes[level]))
            head->curr = level;
    }

    kfree(node);
}
EXPORT_SYMBOL(skiplist_delete);

void *skiplist_find(struct skip_head *head, void *key,
                    skiplist_find_t find)
{
    struct skip_node *node;
    node = skipnode_find(head, key, find, NULL);
    return node ? node->pdata : NULL;
}
EXPORT_SYMBOL(skiplist_find);

static void skiplist_release(struct skip_head *head, skiplist_release_t relse)
{
    struct skip_node *node, *tmp;
    list_for_each_entry_safe(node, tmp, head->nodes, list[0]) {
        if (relse)
            relse(node->pdata);
        kfree(node);
    }
}

void skiplist_reset(struct skip_head *head, skiplist_release_t relse)
{
    unsigned int count;

    skiplist_release(head, relse);
    for (count = 0; count < head->levels; ++count)
        list_head_init(&head->nodes[count]);

    head->curr = 0;
}
EXPORT_SYMBOL(skiplist_reset);

void skiplist_destroy(struct skip_head *head, skiplist_release_t relse)
{
    skiplist_release(head, relse);
    kfree(head);
}
EXPORT_SYMBOL(skiplist_destroy);

struct skip_head *skiplist_create(unsigned int levels, gfp_t flags)
{
    struct skip_head *head;
    unsigned int count;

    head = kmalloc(sizeof(*head) + sizeof(*head->nodes) * levels, flags);
    if (unlikely(!head))
        return NULL;

    for (count = 0; count < levels; ++count)
        list_head_init(&head->nodes[count]);

    head->curr = 0;
    head->levels = levels;

    return head;
}
EXPORT_SYMBOL(skiplist_create);
