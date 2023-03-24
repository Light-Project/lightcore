/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_

#include <ktime.h>
#include <rbtree.h>

struct tqueue_node {
    struct rb_node node;
    ktime_t timer;
};

struct tqueue_head {
    struct rb_root_cached root;
};

extern bool tqueue_insert(struct tqueue_head *head, struct tqueue_node *node);
extern bool tqueue_delete(struct tqueue_head *head, struct tqueue_node *node);

extern struct tqueue_node *tqueue_first(struct tqueue_head *head);
extern struct tqueue_node *tqueue_last(struct tqueue_head *head);
extern struct tqueue_node *tqueue_next(struct tqueue_node *node);
extern struct tqueue_node *tqueue_prev(struct tqueue_node *node);

static inline void tqueue_init(struct tqueue_head *head)
{
    head->root = RB_CACHED_INIT;
}

static inline void tqueue_node_init(struct tqueue_node *node)
{
    RB_CLEAR_NODE(&node->node);
}

/**
 * tqueue_entry - get the struct for this entry or null.
 * @ptr: the &struct tqueue_node pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_entry(ptr, type, member) \
    container_of_safe(ptr, type, member)

/**
 * tqueue_first_entry - get the first element from a timerqueue.
 * @ptr: the timerqueue head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_first_entry(ptr, type, member) \
    tqueue_entry(tqueue_first(ptr), type, member)

/**
 * tqueue_last_entry - get the last element from a timerqueue.
 * @ptr: the timerqueue head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_last_entry(ptr, type, member) \
    tqueue_entry(tqueue_last(ptr), type, member)

/**
 * tqueue_next_entry - get the next element in timerqueue.
 * @pos: the type * to cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_next_entry(pos, member) \
    tqueue_entry(tqueue_next(&(pos)->member), typeof(*(pos)), member)

/**
 * tqueue_prev_entry - get the prev element in timerqueue.
 * @pos: the type * to cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_prev_entry(pos, member) \
    tqueue_entry(tqueue_prev(&(pos)->member), typeof(*(pos)), member)

/**
 * tqueue_for_each - iterate over a timerqueue.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 * @head: the head for your timerqueue.
 */
#define tqueue_for_each(pos, head) \
    for (pos = tqueue_first(head); pos; pos = tqueue_next(pos))

/**
 * tqueue_for_each_reverse - iterate over a timerqueue backwards.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 * @head: the head for your timerqueue.
 */
#define tqueue_for_each_reverse(pos, head) \
    for (pos = tqueue_last(head); pos; pos = tqueue_prev(pos))

/**
 * tqueue_for_each_from - iterate over a timerqueue from the current point.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 */
#define tqueue_for_each_from(pos) \
    for (; pos; pos = tqueue_next(pos))

/**
 * tqueue_for_each_reverse_from - iterate over a timerqueue backwards from the current point.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 */
#define tqueue_for_each_reverse_from(pos) \
    for (; pos; pos = tqueue_prev(pos))

/**
 * tqueue_for_each_continue - continue iteration over a timerqueue.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 */
#define tqueue_for_each_continue(pos) \
    for (pos = tqueue_next(pos); pos; pos = tqueue_next(pos))

/**
 * tqueue_for_each_reverse_continue - continue iteration over a timerqueue backwards.
 * @pos: the &struct tqueue_node to use as a loop cursor.
 */
#define tqueue_for_each_reverse_continue(pos) \
    for (pos = tqueue_prev(pos); pos; pos = tqueue_prev(pos))

/**
 * tqueue_for_each_entry - iterate over timerqueue of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your timerqueue.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry(pos, head, member) \
    for (pos = tqueue_first_entry(head, typeof(*pos), member); \
         pos; pos = tqueue_next_entry(pos, member))

/**
 * tqueue_for_each_entry_reverse - iterate backwards over timerqueue of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your timerqueue.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry_reverse(pos, head, member) \
    for (pos = tqueue_last_entry(head, typeof(*pos), member); \
         pos; pos = tqueue_prev_entry(pos, member))

/**
 * tqueue_for_each_entry_from - iterate over timerqueue of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry_from(pos, member) \
    for (; pos; pos = tqueue_next_entry(pos, member))

/**
 * tqueue_for_each_entry_reverse_from - iterate backwards over timerqueue of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry_reverse_from(pos, member) \
    for (; pos; pos = tqueue_prev_entry(pos, member))

/**
 * tqueue_for_each_entry_continue - continue iteration over timerqueue of given type.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry_continue(pos, member) \
    for (pos = tqueue_next_entry(pos, member); pos; \
         pos = tqueue_next_entry(pos, member))

/**
 * tqueue_for_each_entry_reverse_continue - iterate backwards from the given point.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the tqueue_node within the struct.
 */
#define tqueue_for_each_entry_reverse_continue(pos, member) \
    for (pos = tqueue_prev_entry(pos, member); pos; \
         pos = tqueue_prev_entry(pos, member))

#endif /* _TIMERQUEUE_H_ */
