/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HLIST_H_
#define _HLIST_H_

#include <types.h>
#include <stddef.h>
#include <kernel.h>
#include <poison.h>

struct hlist_node {
    struct hlist_node *next;
    struct hlist_node **pprev;
};

struct hlist_head {
    struct hlist_node *node;
};

#define HLIST_HEAD_STATIC \
    {NULL}

#define HLIST_HEAD_INIT \
    (struct hlist_head) HLIST_HEAD_STATIC

#define HLIST_HEAD(name) \
    struct hlist_head name = HLIST_HEAD_INIT

/* Prevent head only enabling debug */
#if defined(__KERNEL__) && defined(CONFIG_DEBUG_HLIST)
extern bool hlist_debug_head_add_check(struct hlist_head *head, struct hlist_node *new);
extern bool hlist_debug_next_add_check(struct hlist_node *next, struct hlist_node *new);
extern bool hlist_debug_prev_add_check(struct hlist_node *prev, struct hlist_node *new);
extern bool hlist_debug_del_check(struct hlist_node *node);
#define DEBUG_HLIST
#endif

/**
 * hlist_head_init - initialize a hlist_head structure.
 * @head: list_head structure to be initialized.
 */
static inline void hlist_head_init(struct hlist_head *head)
{
    head->node = NULL;
}

/**
 * hlist_node_init - initialize a hlist_node structure.
 * @node: hlist_node structure to be initialized.
 */
static inline void hlist_node_init(struct hlist_node *node)
{
    node->pprev = NULL;
    node->next = NULL;
}

/**
 * hlist_head_add - add a new entry at the beginning of the hlist.
 * @head: hlist head to add it after.
 * @new: new entry to be added.
 */
static inline void hlist_head_add(struct hlist_head *head, struct hlist_node *new)
{
#ifdef DEBUG_HLIST
    if (unlikely(!hlist_debug_head_add_check(head, new)))
        return;
#endif

    new->next = head->node;
    new->pprev = &head->node;

    if (head->node)
        head->node->pprev = &new->next;
    head->node = new;
}

/**
 * hlist_next_add - add a new entry before the one specified.
 * @node: hlist node to add it after, which must be non-NULL.
 * @new: new entry to be added.
 */
static inline void hlist_next_add(struct hlist_node *node, struct hlist_node *new)
{
#ifdef DEBUG_HLIST
    if (unlikely(!hlist_debug_next_add_check(node, new)))
        return;
#endif

    new->pprev = &node->next;
    new->next = node->next;

    if (new->next)
        new->next->pprev = &node->next;
    node->next = new;
}

/**
 * hlist_prev_add - add a new entry before the one specified.
 * @node: hlist node to add it before, which must be non-NULL.
 * @new: new entry to be added.
 */
static inline void hlist_prev_add(struct hlist_node *node, struct hlist_node *new)
{
#ifdef DEBUG_HLIST
    if (unlikely(!hlist_debug_prev_add_check(node, new)))
        return;
#endif

    new->pprev = node->pprev;
    new->next = node;
    node->pprev = &node->next;
    *node->pprev = new;
}

/**
 * hlist_deluf - delete the specified hlist_node from its list (unsafe).
 * @node: the element to delete from the list.
 */
static inline void hlist_deluf(struct hlist_node *node)
{
    struct hlist_node **pprev = node->pprev;
    struct hlist_node *next = node->next;

    if (next)
        next->pprev = pprev;
    *pprev = next;
}

/**
 * hlist_del - delete the specified hlist_node from its list.
 * @node: the element to delete from the list.
 */
static inline void hlist_del(struct hlist_node *node)
{
#ifdef DEBUG_HLIST
    if (unlikely(!hlist_debug_del_check(node)))
        return;
#endif

    hlist_deluf(node);
    node->next = POISON_HLIST1;
    node->pprev = POISON_HLIST2;
}

/**
 * hlist_del_init - delete the specified hlist_node from its list and initialize.
 * @node: the element to delete from the list.
 */
static inline void hlist_del_init(struct hlist_node *node)
{
    hlist_deluf(node);
    hlist_node_init(node);
}

/**
 * hlist_check_empty - check whether the node is head.
 * @head: hlist head to check.
 */
static inline bool hlist_check_empty(const struct hlist_head *head)
{
    return !head->node;
}

/**
 * hlist_check_first - check whether the node is a header.
 * @head: the head of the hlist.
 * @node: the entry to test.
 */
static inline bool hlist_check_first(const struct hlist_head *head, const struct hlist_node *node)
{
    return head->node == node;
}

/**
 * hlist_check_end - check whether the node is a ending.
 * @head: the head of the hlist.
 * @node: the entry to test.
 */
static inline bool hlist_check_end(const struct hlist_node *node)
{
    return !node->next;
}

/**
 * hlist_check_another - check whether has another node.
 * @head: hlist head to check.
 * @node: the unique node.
 */
static inline bool hlist_check_another(const struct hlist_head *head, const struct hlist_node *node)
{
    return head->node == node && node->next == NULL;
}

/**
 * hlist_check_outsize - check whether the node is outside the list.
 * @node: list entry to check.
 */
static inline bool hlist_check_outsize(const struct hlist_node *node)
{
    return node->next == POISON_HLIST1;
}

/**
 * hlist_check_unhashed - check whether the node is reinitialized.
 * @node: hlist node to check.
 */
static inline bool hlist_check_unhashed(const struct hlist_node *node)
{
    return !node->pprev;
}

/**
 * hlist_move_list - move an hlist.
 * @old: hlist_head for old list.
 * @new: hlist_head for new list.
 */
static inline void hlist_move_list(struct hlist_head *old, struct hlist_head *new)
{
    new->node = old->node;
    old->node = NULL;
    if (new->node)
        new->node->pprev = &new->node;
}

/**
 * hlist_entry - get the struct for this entry.
 * @ptr: the &struct hlist_node pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define hlist_entry(ptr, type, member) \
    container_of_safe(ptr, type, member)

/**
 * hlist_first_entry - get the first element from a list.
 * @head: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define hlist_first_entry(head, type, member) \
    hlist_entry((head)->node, type, member)

/**
 * hlist_next_entry - get the next element in list.
 * @pos: the type * to cursor.
 * @member: the name of the hlist_node within the struct.
 */
#define hlist_next_entry(pos, member) \
    hlist_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * hlist_for_each - iterate over a hlist.
 * @pos: the &struct hlist_node to use as a loop cursor.
 * @head: the head for your hlist.
 */
#define hlist_for_each(pos, head) \
    for ((pos) = (head)->node; (pos); (pos) = (pos)->next)

/**
 * hlist_for_each_from - iterate over a hlist from the current point.
 * @pos: the &struct hlist_node to use as a loop cursor.
 */
#define hlist_for_each_from(pos) \
    for (; (pos); (pos) = (pos)->next)

/**
 * hlist_for_each_continue - continue iteration over a hlist.
 * @pos: the &struct hlist_node to use as a loop cursor.
 */
#define hlist_for_each_continue(pos) \
    for ((void)((pos) && ((pos) = (pos)->next)); (pos); (pos) = (pos)->next)

/**
 * hlist_for_each_safe - iterate over a hlist safe against removal of hlist entry.
 * @pos: the &struct hlist_node to use as a loop cursor.
 * @tmp: another hlist_node to use as temporary storage.
 * @head: the head for your hlist.
 */
#define hlist_for_each_safe(pos, tmp, head) \
    for ((pos) = (head)->node; (pos) && ({(tmp) = (pos)->next; 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * hlist_for_each_from_safe - iterate over a hlist safe against removal of hlist entry from the current point.
 * @pos: the &struct hlist_node to use as a loop cursor.
 * @tmp: another hlist_node to use as temporary storage.
 */
#define hlist_for_each_from_safe(pos, tmp) \
    for (; (pos) && ({(tmp) = (pos)->next; 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * hlist_for_each_continue_safe - continue hlist iteration safe against removal.
 * @pos: the &struct hlist_node to use as a loop cursor.
 * @tmp: another hlist_node to use as temporary storage.
 */
#define hlist_for_each_continue_safe(pos, tmp) \
    for ((void)((pos) && ((pos) = (pos)->next)); \
         (pos) && ({(tmp) = (pos)->next; 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * hlist_for_each_entry	- iterate over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member) \
    for ((pos) = hlist_first_entry(head, typeof(*(pos)), member); \
         (pos); (pos) = hlist_next_entry(pos, member))

/**
 * hlist_for_each_entry_from - continue iteration over hlist of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(pos, member) \
    for (; (pos); (pos) = hlist_next_entry(pos, member))

/**
 * hlist_for_each_entry_continue - continue iteration over hlist of given type.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(pos, member) \
    for ((void)((pos) && ((pos) = hlist_next_entry(pos, member))); \
         (pos); (pos) = hlist_next_entry(pos, member))

/**
 * hlist_for_each_entry_safe - iterate over hlist of given type safe against removal of hlist entry
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage
 * @head: the head for your hlist.
 * @member: the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(pos, tmp, head, member) \
    for ((pos) = hlist_first_entry(head, typeof(*(pos)), member); \
         (pos) && ({(tmp) = hlist_next_entry(pos, member); 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = hlist_next_entry(tmp, member))))

/**
 * hlist_for_each_entry_from_safe - iterate over hlist from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from_safe(pos, tmp, member) \
    for (; (pos) && ({(tmp) = hlist_next_entry(pos, member); 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = hlist_next_entry(pos, member))))

/**
 * hlist_for_each_entry_continue_safe - continue hlist iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue_safe(pos, tmp, member) \
    for ((void)((pos) && ((pos) = hlist_next_entry(pos, member))); \
         (pos) && ({(tmp) = hlist_next_entry(pos, member); 1;}); \
         (pos) = (tmp), ((tmp) && ((tmp) = hlist_next_entry(pos, member))))

#endif /* _HLIST_H_ */
