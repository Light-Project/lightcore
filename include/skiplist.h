/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <list.h>

typedef long (*skiplist_find_t)(const void *data, const void *key);
typedef long (*skiplist_cmp_t)(const void *dataa, const void *datab);
typedef void (*skiplist_release_t)(void *data);

struct skip_node {
    void *pdata;
    struct list_head list[0];
};

struct skip_head {
    unsigned int curr;
    unsigned int levels;
    struct list_head nodes[0];
};

extern state skiplist_insert(struct skip_head *head, void *data, skiplist_cmp_t cmp, gfp_t flags);
extern void skiplist_delete(struct skip_head *head, void *key, skiplist_find_t find);
extern void *skiplist_find(struct skip_head *head, void *key, skiplist_find_t find);

extern void skiplist_reset(struct skip_head *head, skiplist_release_t relse);
extern void skiplist_destroy(struct skip_head *head, skiplist_release_t relse);
extern struct skip_head *skiplist_create(unsigned int levels, gfp_t flags);

/**
 * skip_for_each - iterate over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each(pos, head, level) \
    list_for_each_entry(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse - iterate backwards over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each_reverse(pos, head, level) \
    list_for_each_entry_reverse(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_from - iterate over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each_from(pos, head, level) \
    list_for_each_entry_from(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse_from - iterate backwards over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each_reverse_from(pos, head, level) \
    list_for_each_entry_reverse_from(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_continue - continue iteration over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each_continue(pos, head, level) \
    list_for_each_entry_continue(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse_continue - iterate backwards from the given point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 */
#define skip_for_each_reverse_continue(pos, head, level) \
    list_for_each_entry_reverse_continue(pos, &(head)->nodes[level], list[level])

/**
 * skip_for_each_safe - iterate over list of given type safe against removal of list entry.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_safe(pos, tmp, head, level) \
    list_for_each_entry_safe(pos, tmp, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse_safe - iterate backwards over list safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_reverse_safe(pos, tmp, head, level) \
    list_for_each_entry_reverse_safe(pos, tmp, &(head)->nodes[level], list[level])

/**
 * skip_for_each_from_safe - iterate over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_from_safe(pos, tmp, head, level) \
    list_for_each_entry_from_safe(pos, tmp, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse_from_safe - iterate backwards over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_reverse_from_safe(pos, tmp, head, level) \
    list_for_each_entry_reverse_from_safe(pos, tmp, &(head)->nodes[level], list[level])

/**
 * skip_for_each_continue_safe - continue list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_continue_safe(pos, tmp, head, level) \
    list_for_each_entry_continue_safe(pos, tmp, &(head)->nodes[level], list[level])

/**
 * skip_for_each_reverse_continue_safe - continue backwards over list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 */
#define skip_for_each_reverse_continue_safe(pos, tmp, head, level) \
    list_for_each_entry_reverse_continue_safe(pos, tmp, &(head)->nodes[level], list[level])

#endif  /* _SKIPLIST_H_ */
