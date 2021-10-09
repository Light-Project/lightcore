/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HLIST_H_
#define _HLIST_H_

#include <stddef.h>
#include <types.h>
#include <asm/rwonce.h>

struct hlist_node {
    struct hlist_node *next, **pprev;
};

struct hlist_head {
    struct hlist_node *node;
};

#define HLIST_HEAD(name) \
    hlist_head_t name = {NULL};

static inline void hlist_node_init(struct hlist_node *node)
{
    node->next = NULL;
    node->pprev = NULL;
}

/**
 * hlist_empty - Is the specified hlist_head structure an empty hlist?
 * @head: Structure to check.
 */
static inline bool hlist_empty(struct hlist_head *head)
{
    return !READ_ONCE(head->node);
}

/**
 * hlist_unhashed - Has node been removed from list and reinitialized?
 * @node: Node to be checked
 */
static inline bool hlist_unhashed(struct hlist_node *node)
{
    return !READ_ONCE(node->pprev);
}

/**
 * hlist_add_head - add a new entry at the beginning of the hlist
 * @head: hlist head to add it after
 * @new: new entry to be added
 */
static inline void hlist_add_head(struct hlist_head *head, struct hlist_node *new)
{
    WRITE_ONCE(new->pprev, &head->node);
    WRITE_ONCE(new->next, head->node);
    if (head->node)
		WRITE_ONCE(head->node->pprev, &new->next);

    WRITE_ONCE(head->node, new);
}

/**
 * hlist_add_prev - add a new entry before the one specified
 * @node: hlist node to add it before, which must be non-NULL
 * @new: new entry to be added
 */
static inline void hlist_add_prev(struct hlist_node *node, struct hlist_node *new)
{
    WRITE_ONCE(new->pprev, node->pprev);
    WRITE_ONCE(new->next, node);
    WRITE_ONCE(node->pprev, &new->next);
    WRITE_ONCE(*(new->pprev), new);
}

/**
 * hlist_add_next - add a new entry after the one specified
 * @node: hlist node to add it after, which must be non-NULL
 * @new: new entry to be added
 */
static inline void hlist_add_next(struct hlist_node *node, struct hlist_node *new)
{
    WRITE_ONCE(new->pprev, &node->pprev);
    WRITE_ONCE(new->next, node->next);
    WRITE_ONCE(node->next, node);

    if (new->next)
        WRITE_ONCE(new->next->pprev, &new->next);
}

static inline void hlist_del(struct hlist_node *node)
{
    struct hlist_node *next = node->next;
    struct hlist_node **pprev = node->pprev;

    WRITE_ONCE(*pprev, next);
    if (next)
        WRITE_ONCE(next->pprev, pprev);
}

#define hlist_entry(ptr, type, member) \
    container_of(ptr,type,member)

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

#endif
