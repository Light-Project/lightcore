/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SLIST_H_
#define _SLIST_H_

#include <types.h>
#include <stddef.h>
#include <kernel.h>
#include <poison.h>

struct slist_head {
    struct slist_head *next;
};

#define SLIST_HEAD_STATIC \
    {NULL}

#define SLIST_HEAD_INIT \
    (struct slist_head) SLIST_HEAD_STATIC

#define SLIST_HEAD(name) \
    struct slist_head name = SLIST_HEAD_INIT

#ifdef CONFIG_DEBUG_SLIST
extern bool slist_debug_add_check(struct slist_head *node, struct slist_head *new);
extern bool slist_debug_del_check(struct slist_head *node);
#define DEBUG_SLIST
#endif

/**
 * slist_head_init - initialize a slist_head structure.
 * @head: slist_head structure to be initialized.
 */
static inline void slist_head_init(struct slist_head *head)
{
    head->next = NULL;
}

/**
 * slist_add - add a new node next old node.
 * @node: list head to add it next.
 * @new: new entry to be added.
 */
static inline void slist_add(struct slist_head *node, struct slist_head *new)
{
#ifdef DEBUG_SLIST
    if (unlikely(!slist_debug_add_check(node, new)))
        return;
#endif
    new->next = node->next;
    node->next = new;
}

/**
 * slist_del - delete a node from slist.
 * @head: the head of the slist.
 * @entry: the element to delete from the slist.
 */
static inline void slist_del(struct slist_head *head, struct slist_head *node)
{
    struct slist_head *walk = head;

#ifdef DEBUG_SLIST
    if (unlikely(!slist_debug_del_check(node)))
        return;
#endif

    while (walk->next != node)
        walk = walk->next;

    walk->next = node->next;
    node->next = POISON_SLIST;
}

/**
 * slist_replace - replace a linked slist node with an external node.
 * @head: the head of the slist.
 * @old: the element to be replaced.
 * @new: the new element to insert.
 */
static inline void slist_replace(struct slist_head *head, struct slist_head *old, struct slist_head *new)
{
    struct slist_head *walk = head;

    while (walk->next != old)
        walk = walk->next;

    walk->next = new;
    new->next = old->next;
}

/**
 * slist_check_empty - check whether the node is head.
 * @head: slist head to check.
 */
static inline bool slist_check_empty(const struct slist_head *head)
{
    return !head->next;
}

/**
 * slist_check_first - check whether the node is a header.
 * @head: the head of the slist.
 * @node: the entry to test.
 */
static inline bool slist_check_first(const struct slist_head *head, const struct slist_head *node)
{
    return head->next == node;
}

/**
 * slist_check_next - check whether the node have next.
 * @node: the node to check.
 */
static inline bool slist_check_next(const struct slist_head *node)
{
    return node->next;
}

/**
 * slist_check_another - check whether has another node.
 * @head: slist head to check.
 * @node: the unique node.
 */
static inline bool slist_check_another(const struct slist_head *head, const struct slist_head *node)
{
    return head->next == node && node->next == NULL;
}

/**
 * slist_entry - get the struct for this entry.
 * @ptr: the &struct slist_head pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define slist_entry(ptr, type, member) \
    container_of_safe(ptr, type, member)

/**
 * slist_first_entry - get the first element from a slist.
 * @ptr: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member:	the name of the slist_head within the struct.
 */
#define slist_first_entry(ptr, type, member) \
    slist_entry((ptr)->next, type, member)

/**
 * slist_next_entry - get the next element in slist.
 * @pos: the type * to cursor.
 * @member: the name of the slist_head within the struct.
 */
#define slist_next_entry(pos, member) \
    slist_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * slist_for_each - iterate over a slist.
 * @pos: the &struct slist_head to use as a loop cursor.
 * @head: the head for your slist.
 */
#define slist_for_each(pos, head) \
    for ((pos) = (head)->next; (pos); (pos) = (pos)->next)

/**
 * slist_for_each_from - iterate over a slist from the current point.
 * @pos: the &struct slist_head to use as a loop cursor.
 */
#define slist_for_each_from(pos) \
    for (; (pos); (pos) = (pos)->next)

/**
 * slist_for_each_continue - continue iteration over a slist.
 * @pos: the &struct slist_head to use as a loop cursor.
 */
#define slist_for_each_continue(pos) \
    for ((pos) = (pos)->next; (pos); (pos) = (pos)->next)

/**
 * slist_for_each_safe - iterate over a slist safe against removal of slist entry.
 * @pos: the &struct slist_head to use as a loop cursor.
 * @tmp: another slist_head to use as temporary storage.
 * @head: the head for your slist.
 */
#define slist_for_each_safe(pos, tmp, head) \
    for ((pos) = (head)->next, (tmp) = (pos)->next; \
         (pos); (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * slist_for_each_from_safe - iterate over a slist safe against removal of slist entry from the current point.
 * @pos: the &struct slist_head to use as a loop cursor.
 * @tmp: another slist_head to use as temporary storage.
 */
#define slist_for_each_from_safe(pos, tmp) \
    for ((tmp) = (pos)->next; \
         (pos); (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * slist_for_each_continue_safe - continue slist iteration safe against removal.
 * @pos: the &struct slist_head to use as a loop cursor.
 * @tmp: another slist_head to use as temporary storage.
 */
#define slist_for_each_continue_safe(pos, tmp) \
    for ((pos) = (pos)->next, (tmp) = (pos)->next; \
         (pos); (pos) = (tmp), ((tmp) && ((tmp) = (tmp)->next)))

/**
 * slist_for_each_entry - iterate over slist of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your slist.
 * @member: the name of the slist_head within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for ((pos) = slist_first_entry(head, typeof(*(pos)), member); \
         (pos); (pos) = slist_next_entry(pos, member))

/**
 * slist_for_each_entry_from - iterate over slist of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the slist_head within the struct.
 */
#define slist_for_each_entry_from(pos, member) \
    for (; (pos); (pos) = slist_next_entry(pos, member))

/**
 * slist_for_each_entry_continue - continue iteration over slist of given type.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the slist_head within the struct.
 */
#define slist_for_each_entry_continue(pos, member) \
    for ((pos) = slist_next_entry(pos, member); \
         (pos); (pos) = slist_next_entry(pos, member))

/**
 * slist_for_each_entry_safe - iterate over slist of given type safe against removal of slist entry.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your slist.
 * @member: the name of the slist_head within the struct.
 */
#define slist_for_each_entry_safe(pos, tmp, head, member) \
    for ((pos) = slist_first_entry(head, typeof(*(pos)), member), \
         (tmp) = slist_next_entry(pos, member); (pos); (pos) = (tmp), \
         ((tmp) && ((tmp) = slist_next_entry(tmp, member))))

/**
 * slist_for_each_entry_from_safe - iterate over slist from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @member:	the name of the slist_head within the struct.
 */
#define slist_for_each_entry_from_safe(pos, tmp, member) \
    for ((tmp) = slist_next_entry(pos, member); (pos); (pos) = (tmp), \
         ((tmp) && ((tmp) = slist_next_entry(pos, member))))

/**
 * slist_for_each_entry_continue_safe - continue slist iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @member:	the name of the slist_head within the struct.
 */
#define slist_for_each_entry_continue_safe(pos, tmp, member) \
    for ((pos) = slist_next_entry(pos, member), \
         (tmp) = slist_next_entry(pos, member); (pos); (pos) = (tmp), \
         ((tmp) && ((tmp) = slist_next_entry(pos, member))))

#endif /* _SLIST_H_ */
