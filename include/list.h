/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIST_H_
#define _LIST_H_

#include <types.h>
#include <stddef.h>
#include <kernel.h>
#include <poison.h>

struct list_head {
    struct list_head *prev, *next;
};

#define LIST_HEAD_STATIC(name) \
    {&(name), &(name)}

#define LIST_HEAD_INIT(name) \
    (struct list_head) LIST_HEAD_STATIC(name)

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

typedef long (*list_cmp_t)(struct list_head *, struct list_head *, void *);
extern void list_qsort(struct list_head *head, list_cmp_t cmp, void *data);
extern void list_bsort(struct list_head *head, list_cmp_t cmp, void *data);

#if defined(__KERNEL__) && defined(CONFIG_DEBUG_LIST)
extern bool list_debug_add_check(struct list_head *prev, struct list_head *next, struct list_head *new);
extern bool list_debug_del_check(struct list_head *node);
#define DEBUG_LIST
#endif

static __always_inline void
list_insert(struct list_head *prev, struct list_head *next, struct list_head *new)
{
#ifdef DEBUG_LIST
    if (unlikely(!list_debug_add_check(prev, next, new)))
        return;
#endif

    prev->next = new;
    next->prev = new;
    new->prev = prev;
    new->next = next;
}

/**
 * list_head_init - initialize a list_head structure.
 * @head: list_head structure to be initialized.
 */
static inline void list_head_init(struct list_head *head)
{
    head->prev = head;
    head->next = head;
}

/**
 * list_add - add a new node next old node.
 * @node: list head to add it next.
 * @new: new entry to be added.
 */
static inline void list_add(struct list_head *node, struct list_head *new)
{
    list_insert(node, node->next, new);
}

/**
 * list_add_prev - add a new node after old node.
 * @node: list head to add it prev.
 * @new: new entry to be added.
 */
static inline void list_add_prev(struct list_head *node, struct list_head *new)
{
    list_insert(node->prev, node, new);
}

/**
 * list_del - in fact, it just connect next and prev node.
 * @node: the element to delete from the list.
 */
static inline void list_del(struct list_head *node)
{
#ifdef DEBUG_LIST
    if (unlikely(!list_debug_del_check(node)))
        return;
#endif

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = POISON_LIST1;
    node->prev = POISON_LIST2;
}

/**
 * list_replace - replace a linked list node with an external node.
 * @old: the element to be replaced.
 * @new: the new element to insert.
 */
static inline void list_replace(struct list_head *old, struct list_head *new)
{
    new->prev = old->prev;
    new->next = old->next;
    new->prev->next = new;
    new->next->prev = new;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @node: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *node)
{
    list_del(node);
    list_head_init(node);
}

/**
 * list_replace_init - replace old entry by new one and initialize the old one.
 * @old: the element to be replaced.
 * @new: the new element to insert.
 */
static inline void list_replace_init(struct list_head *old, struct list_head *new)
{
    list_replace(old, new);
    list_head_init(old);
}

/**
 * list_move_front - move the node to the front of the list.
 * @head: the head that will precede our entry.
 * @node: the entry to move.
 */
static inline void list_move_front(struct list_head *head, struct list_head *node)
{
    list_del(node);
    list_add(head, node);
}

/**
 * list_move_tail - move the node to the tail of the list.
 * @head: the head that will follow our entry.
 * @node: the entry to move.
 */
static inline void list_move_tail(struct list_head *head, struct list_head *node)
{
    list_del(node);
    list_add_prev(head, node);
}

/**
 * list_check_head - check whether the node is head.
 * @head: the head of the list
 * @list: the entry to test
 */
static inline bool list_check_head(const struct list_head *head, const struct list_head *node)
{
    return head == node;
}

/**
 * list_check_first - check whether the node is a header.
 * @head: the head of the list.
 * @node: the entry to test.
 */
static inline bool list_check_first(const struct list_head *head, const struct list_head *node)
{
    return node->prev == head;
}

/**
 * list_check_end - check whether the node is a ending.
 * @head: the head of the list.
 * @node: the entry to test.
 */
static inline bool list_check_end(const struct list_head *head, const struct list_head *node)
{
    return node->next == head;
}

/**
 * list_check_empty - check whether a list is empty.
 * @head: list head to check.
 */
static inline bool list_check_empty(const struct list_head *head)
{
    return head->next == head;
}

/**
 * list_check_another - check whether has another node.
 * @head: list head to check.
 * @node: the unique node.
 */
static inline bool list_check_another(const struct list_head *head, const struct list_head *node)
{
    return head->next == node && head->prev == node;
}

/**
 * list_check_outsize - check whether the node is outside the linked list.
 * @node: list entry to check.
 */
static inline bool list_check_outsize(const struct list_head *node)
{
    return node->next == POISON_LIST1 &&
           node->prev == POISON_LIST2;
}

/**
 * list_entry - get the struct for this entry.
 * @ptr: the &struct list_head pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * list_entry_check_head - test if the entry points to the head of the list.
 * @pos: the type * to cursor
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_entry_check_head(pos, head, member) \
    (&(pos)->member == (head))

/**
 * list_first_entry - get the first element from a list.
 * @ptr: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list.
 * @ptr: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list or null.
 * @ptr: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_first_entry_or_null(ptr, type, member) ({          \
    struct list_head *head__ = (ptr);                           \
    struct list_head *pos__ = head__->next;                     \
    pos__ != head__ ? list_entry(pos__, type, member) : NULL;   \
})

/**
 * list_last_entry_or_null - get the last element from a list or null.
 * @ptr: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_last_entry_or_null(ptr, type, member) ({           \
    struct list_head *head__ = (ptr);                           \
    struct list_head *pos__ = head__->prev;                     \
    pos__ != head__ ? list_entry(pos__, type, member) : NULL;   \
})

/**
 * list_next_entry - get the next element in list.
 * @pos: the type * to cursor
 * @member: the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list.
 * @pos: the type * to cursor
 * @member: the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_for_each - iterate over a list.
 * @head: the head for your list.
 * @pos: the &struct list_head to use as a loop cursor.
 */
#define list_for_each(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); pos = (pos)->next)

/**
 * list_for_each_reverse - iterate over a list backwards.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each_reverse(pos, head) \
    for ((pos) = (head)->prev; (pos) != (head); (pos) = (pos)->prev)

/**
 * list_for_each_from - iterate over a list from the current point.
 * @head: the head for your list.
 * @pos: the &struct list_head to use as a loop cursor.
 */
#define list_for_each_from(pos, head) \
    for (; !list_check_head(pos, head); (pos) = (pos)->next)

/**
 * list_for_each_reverse_from - iterate over a list backwards from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each_reverse_from(pos, head) \
    for (; !list_check_head(pos, head); (pos) = (pos)->prev)

/**
 * list_for_each_continue - continue iteration over a list.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each_continue(pos, head)                                   \
    for ((pos) = (pos)->next; !list_check_head(pos, head);                  \
         (pos) = (pos)->next)

/**
 * list_for_each_reverse_continue - continue iteration over a list backwards.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each_reverse_continue(pos, head)                           \
    for ((pos) = (pos)->prev; !list_check_head(pos, head);                  \
         (pos) = (pos)->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_safe(pos, tmp, head)                                  \
    for ((pos) = (head)->next, (tmp) = (pos)->next;                         \
         !list_check_head((pos), head);                                     \
         (pos) = (tmp), (tmp) = (tmp)->next)

/**
 * list_for_each_reverse_safe - iterate backwards over list safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_reverse_safe(pos, tmp, head)                          \
    for ((pos) = (head)->prev, (tmp) = (pos)->prev;                         \
         !list_check_head(pos, head);                                       \
         (pos) = (tmp), (tmp) = (tmp)->prev)

/**
 * list_for_each_from_safe - iterate over a list safe against removal of list entry from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_from_safe(pos, tmp, head)                             \
    for ((tmp) = (pos)->next;                                               \
         !list_check_head(pos, head);                                       \
         (pos) = (tmp), (tmp) = (tmp)->next)

/**
 * list_for_each_reverse_from_safe - iterate backwards over list safe against removal from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_reverse_from_safe(pos, tmp, head)                     \
    for ((tmp) = pos->prev;                                                 \
         !list_check_head(pos, head);                                       \
         (pos) = (tmp), (tmp) = (tmp)->prev)

/**
 * list_for_each_continue_safe - continue list iteration safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_continue_safe(pos, tmp, head)                         \
    for ((pos) = (pos)->next, (tmp) = (pos)->next;                          \
         !list_check_head(pos, head);                                       \
         (pos) = (tmp), (tmp) = (tmp)->next)

/**
 * list_for_each_reverse_continue_safe - continue backwards over list iteration safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define list_for_each_reverse_continue_safe(pos, tmp, head)                 \
    for ((pos) = pos->prev, (tmp) = pos->prev;                              \
         !list_check_head(pos, head);                                       \
         (pos) = (tmp), (tmp) = (tmp)->prev)

/**
 * list_for_each_entry - iterate over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)                              \
    for ((pos) = list_first_entry(head, typeof(*(pos)), member);            \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = list_next_entry(pos, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)                      \
    for ((pos) = list_last_entry(head, typeof(*(pos)), member);             \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_from(pos, head, member)                         \
    for (; !list_entry_check_head(pos, head, member);                       \
         (pos) = list_next_entry(pos, member))

/**
 * list_for_each_entry_reverse_from - iterate backwards over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse_from(pos, head, member)                 \
    for (; !list_entry_check_head(pos, head, member);                       \
         (pos) = list_prev_entry(pos, member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_continue(pos, head, member)                     \
    for ((pos) = list_next_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = list_next_entry(pos, member))

/**
 * list_for_each_entry_reverse_continue - iterate backwards from the given point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse_continue(pos, head, member)             \
    for ((pos) = list_prev_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = list_prev_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, tmp, head, member)                    \
    for ((pos) = list_first_entry(head, typeof(*(pos)), member),            \
         (tmp) = list_next_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_next_entry(tmp, member))

/**
 * list_for_each_entry_reverse_safe - iterate backwards over list safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse_safe(pos, tmp, head, member)            \
    for ((pos) = list_last_entry(head, typeof(*pos), member),               \
         (tmp) = list_prev_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_prev_entry(tmp, member))

/**
 * list_for_each_entry_from_safe - iterate over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_from_safe(pos, tmp, head, member)               \
    for ((tmp) = list_next_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_next_entry(tmp, member))

/**
 * list_for_each_entry_reverse_from_safe - iterate backwards over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse_from_safe(pos, tmp, head, member)       \
    for ((tmp) = list_prev_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_prev_entry(tmp, member))

/**
 * list_for_each_entry_continue_safe - continue list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_continue_safe(pos, tmp, head, member)           \
    for ((pos) = list_next_entry(pos, member),                              \
         (tmp) = list_next_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_next_entry(tmp, member))

/**
 * list_for_each_entry_reverse_continue_safe - continue backwards over list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse_continue_safe(pos, tmp, head, member)   \
    for ((pos) = list_prev_entry(pos, member),                              \
         (tmp) = list_prev_entry(pos, member);                              \
         !list_entry_check_head(pos, head, member);                         \
         (pos) = (tmp), (tmp) = list_prev_entry(tmp, member))

#endif  /* _LIST_H_ */
