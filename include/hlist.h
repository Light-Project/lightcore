/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HLIST_H_
#define _HLIST_H_

#include <types.h>
#include <stddef.h>

struct hlist_node {
    struct hlist_node *next;
    struct hlist_node **pprev;
};

struct hlist_head {
    struct hlist_node *node;
};

#define HLIST_HEAD(name) \
    struct hlist_head name = {NULL};

#define hlist_entry(ptr, type, member) \
    container_of(ptr,type,member)

static inline void hlist_head_init(struct hlist_head *head)
{
    head->node = NULL;
}

static inline void hlist_node_init(struct hlist_node *node)
{
    node->next = NULL;
    node->pprev = NULL;
}

static inline void hlist_head_add(struct hlist_head *head, struct hlist_node *node)
{
    node->next = head->node;
    if (head->node != NULL)
        head->node->pprev = &node->next;
    head->node = node;
    node->pprev = &head->node;
}

static inline void hlist_prev_add(struct hlist_node *node, struct hlist_node *new)
{
    new->pprev = new->pprev;
    new->next = node;
}

static inline void hlist_next_add(struct hlist_node *node, struct hlist_node *new)
{
    new->next = node->next;
    node->next = new;
    new->pprev = &node->next;
    if(new->next != NULL)
        new->next->pprev = &node->next;
}

/**
 * hlist_empty - Is the specified hlist_head structure an empty hlist?
 * @head: Structure to check.
 */
static inline bool hlist_empty(struct hlist_head *head)
{
    return !head->node;
}

/**
 * hlist_unhashed - Has node been removed from list and reinitialized?
 * @node: Node to be checked
 */
static inline bool hlist_unhashed(struct hlist_node *node)
{
    return !node->pprev;
}

/**
 * hlist_add_head - add a new entry at the beginning of the hlist
 * @head: hlist head to add it after
 * @new: new entry to be added
 */
static inline void hlist_add_head(struct hlist_head *head, struct hlist_node *new)
{
    new->pprev = &head->node;
    new->next = head->node;
    if (head->node)
        head->node->pprev = &new->next;
    head->node = new;
}

/**
 * hlist_add_prev - add a new entry before the one specified
 * @node: hlist node to add it before, which must be non-NULL
 * @new: new entry to be added
 */
static inline void hlist_add_prev(struct hlist_node *node, struct hlist_node *new)
{
    new->pprev = node->pprev;
    new->next = node;
    node->pprev = &new->next;
    *new->pprev = new;
}

/**
 * hlist_add_next - add a new entry after the one specified
 * @node: hlist node to add it after, which must be non-NULL
 * @new: new entry to be added
 */
static inline void hlist_add_next(struct hlist_node *node, struct hlist_node *new)
{
    new->pprev = &node->next;
    new->next = node->next;
    node->next = node;
    if (new->next)
        new->next->pprev = &new->next;
}

static inline void hlist_del(struct hlist_node *node)
{
    struct hlist_node *next = node->next;
    struct hlist_node **pprev = node->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member)                                 \
    for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member); pos;    \
         pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#endif  /* _HLIST_H_ */
