/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ILIST_H_
#define _ILIST_H_

#include <list.h>

struct ilist_head {
    struct list_head node_list;
};

struct ilist_node {
    struct list_head node_list;
    struct list_head index_list;
};

#define ILIST_HEAD_STATIC(name) \
    {LIST_HEAD_STATIC((name).node_list)}

#define ILIST_HEAD_INIT(name) \
    (struct ilist_head) ILIST_HEAD_STATIC(name)

#define ILIST_HEAD(name) \
    struct ilist_head name = ILIST_HEAD_STATIC(name)

#define ILISI_NODE_INIT(name, index)                        \
    (struct ilist_node) {                                   \
        .node_list = LIST_HEAD_INIT((node).node_list),      \
        .index_list = LIST_HEAD_INIT((node).index_list),    \
    }

typedef long (*ilist_cmp_t)(struct ilist_node *inodea, struct ilist_node *inodeb, const void *pdata);
extern void ilist_add(struct ilist_head *ihead, struct ilist_node *inode, ilist_cmp_t cmp, const void *pdata);
extern void ilist_del(struct ilist_head *ihead, struct ilist_node *inode);

/**
 * ilist_head_init - initialize a ilist_head structure.
 * @head: ilist_head structure to be initialized.
 */
static inline void ilist_head_init(struct ilist_head *ihead)
{
    list_head_init(&ihead->node_list);
}

static inline void ilist_node_init(struct ilist_node *inode)
{
    list_head_init(&inode->node_list);
    list_head_init(&inode->index_list);
}

/**
 * ilist_first - return the first node.
 * @ihead: the &struct ilist_head pointer
 */
static inline struct ilist_node *ilist_first(const struct ilist_head *ihead)
{
    return list_first_entry(&ihead->node_list, struct ilist_node, node_list);
}

/**
 * ilist_last - return the last node.
 * @ihead:the &struct ilist_head pointer
 */
static inline struct ilist_node *ilist_last(const struct ilist_head *ihead)
{
    return list_last_entry(&ihead->node_list, struct ilist_node, node_list);
}

/**
 * list_check_empty - check whether a head is empty.
 * @ihead: list head to check.
 */
static inline bool ilist_head_empty(struct ilist_head *ihead)
{
    return list_check_empty(&ihead->node_list);
}

/**
 * list_check_empty - check whether a node index is empty.
 * @inode: list node to check.
 */
static inline bool ilist_node_empty(struct ilist_node *inode)
{
    return list_check_empty(&inode->index_list);
}

/**
 * ilist_entry - get the struct for this entry.
 * @ptr: the &struct ilist_head pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the ilist_head within the struct.
 */
#define ilist_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * ilist_for_each - iterate over a list.
 * @head: the head for your list.
 * @pos: the &struct list_head to use as a loop cursor.
 */
#define ilist_for_each(pos, head) \
    list_for_each(pos, &(head)->node_list)

/**
 * ilist_for_each_reverse - iterate over a list backwards.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse(pos, head) \
    list_for_each_reverse(pos, &(head)->node_list)

/**
 * ilist_for_each_from - iterate over a list from the current point.
 * @head: the head for your list.
 * @pos: the &struct list_head to use as a loop cursor.
 */
#define ilist_for_each_from(pos, head) \
    list_for_each_from(pos, &(head)->node_list)

/**
 * ilist_for_each_reverse_from - iterate over a list backwards from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse_from(pos, head) \
    list_for_each_reverse_from(pos, &(head)->node_list)

/**
 * ilist_for_each_continue - continue iteration over a list.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define ilist_for_each_continue(pos, head) \
    list_for_each_continue(pos, &(head)->node_list)

/**
 * ilist_for_each_reverse_continue - continue iteration over a list backwards.
 * @pos: the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse_continue(pos, head) \
    list_for_each_reverse_continue(pos, &(head)->node_list)

/**
 * ilist_for_each_safe - iterate over a list safe against removal of list entry.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_safe(pos, tmp, head) \
    list_for_each_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_reverse_safe - iterate backwards over list safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse_safe(pos, tmp, head) \
    list_for_each_reverse_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_from_safe - iterate over a list safe against removal of list entry from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_from_safe(pos, tmp, head) \
    list_for_each_from_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_reverse_from_safe - iterate backwards over list safe against removal from the current point.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse_from_safe(pos, tmp, head) \
    list_for_each_reverse_from_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_continue_safe - continue list iteration safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_continue_safe(pos, tmp, head) \
    list_for_each_continue_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_reverse_continue_safe - continue backwards over list iteration safe against removal.
 * @pos: the &struct list_head to use as a loop cursor.
 * @tmp: another list_head to use as temporary storage.
 * @head: the head for your list.
 */
#define ilist_for_each_reverse_continue_safe(pos, tmp, head) \
    list_for_each_reverse_continue_safe(pos, tmp, &(head)->node_list)

/**
 * ilist_for_each_entry - iterate over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry(pos, head, member) \
    list_for_each_entry(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse(pos, head, member) \
    list_for_each_entry_reverse(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_from - iterate over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_from(pos, head, member) \
    list_for_each_entry_from(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse_from - iterate backwards over list of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse_from(pos, head, member) \
    list_for_each_entry_reverse_from(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_continue - continue iteration over list of given type.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_continue(pos, head, member) \
    list_for_each_entry_continue(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse_continue - iterate backwards from the given point.
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse_continue(pos, head, member) \
    list_for_each_entry_reverse_continue(pos, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_safe - iterate over list of given type safe against removal of list entry.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define ilist_for_each_entry_safe(pos, tmp, head, member) \
    list_for_each_entry_safe(pos, tmp, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse_safe - iterate backwards over list safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse_safe(pos, tmp, head, member) \
    list_for_each_entry_reverse_safe(pos, tmp, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_from_safe - iterate over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define ilist_for_each_entry_from_safe(pos, tmp, head, member) \
    list_for_each_entry_from_safe(pos, tmp, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse_from_safe - iterate backwards over list from current point safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse_from_safe(pos, tmp, head, member) \
    list_for_each_entry_reverse_from_safe(pos, tmp, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_continue_safe - continue list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define ilist_for_each_entry_continue_safe(pos, tmp, head, member) \
    list_for_each_entry_continue_safe(pos, tmp, &(head)->node_list, member.node_list)

/**
 * ilist_for_each_entry_reverse_continue_safe - continue backwards over list iteration safe against removal.
 * @pos: the type * to use as a loop cursor.
 * @tmp: another type * to use as temporary storage.
 * @head: the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define ilist_for_each_entry_reverse_continue_safe(pos, tmp, head, member) \
    list_for_each_entry_reverse_continue_safe(pos, tmp, &(head)->node_list, member.node_list)

#endif  /* _ILIST_H_ */
