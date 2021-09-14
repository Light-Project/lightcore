/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIST_H_
#define _LIST_H_

#include <types.h>
#include <stddef.h>
#include <kernel.h>

#ifndef __ASSEMBLY__

typedef struct list_head{
    struct list_head *prev, *next;
} list_t;

#define LIST_HEAD(name) \
    struct list_head name = { &(name), &(name) }

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_entry_is_head - test if the entry points to the head of the list
 * @pos:	the type * to cursor
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry_is_head(pos, head, member) \
    (&pos->member == (head))

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) ({          \
    struct list_head *head__ = (ptr);                           \
    struct list_head *pos__ = head__->next;                     \
    pos__ != head__ ? list_entry(pos__, type, member) : NULL;   \
})

/**
 * list_for_each - iterate over a list
 * @head:	the head for your list.
 * @pos:	the &struct list_head to use as a loop cursor.
 */
#define list_for_each(head, pos) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(head, pos) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)                  \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                                \
         pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)          \
    for (pos = list_first_entry(head, typeof(*pos), member),    \
         n = list_next_entry(pos, member);                      \
         !list_entry_is_head(pos, head, member);                \
         pos = n, n = list_next_entry(n, member))

void list_head_init(struct list_head *list);
void list_insert(struct list_head *prev, struct list_head *next, struct list_head *new);
void list_add(struct list_head *head, struct list_head *new);
void list_add_prev(struct list_head *node, struct list_head *new);
void list_add_next(struct list_head *node,struct list_head *new);
void list_del(struct list_head *node);
void list_replace(struct list_head *old, struct list_head *new);
void list_move_tail(struct list_head *head, struct list_head *node);

bool list_check_first(struct list_head *head, struct list_head *node);
bool list_check_end(struct list_head *head, struct list_head *node);
bool list_check_around(struct list_head *prev, struct list_head *next);
bool list_check_empty(struct list_head *head);

#endif  /* __ASSEMBLY__ */
#endif  /* _LIST_H_ */
