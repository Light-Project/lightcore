/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SLIST_H_
#define _SLIST_H_

#include <types.h>
#include <stddef.h>
#include <kernel.h>

typedef struct slist_head{
    struct slist_head *next;
} slist_t;

#define SLIST_HEAD(name) \
        struct slist_head name = { NULL }
    
static inline void slist_head_init(struct slist_head *head)
{
    head->next = NULL;
}

static inline void slist_add(struct slist_head *node, 
                             struct slist_head *new)
{
    new->next = node->next;
    node->next = new;
}

static inline void slist_del(struct slist_head *head, 
                             struct slist_head *node)
{
    struct slist_head *walk = head;
    
    while(walk->next != node)
        walk = walk->next;
    
    walk->next = node->next;
}

static inline void slist_replace(struct slist_head *head, 
                                 struct slist_head *old, 
                                 struct slist_head *new)
{
    struct slist_head *walk = head;
    
    while(walk->next != old)
        walk = walk->next;
    
    walk->next = new;
    new->next = old->next;
}

static inline bool slist_check_first(struct slist_head *head, 
                                     struct slist_head *node)
{
    return head->next == node;
}

static inline bool slist_check_end(struct slist_head *node)
{
    return node->next == NULL;
}

static inline bool slist_check_empty(struct slist_head *head)
{
    return head->next == NULL;
}

/**
 * slist_entry - get the struct for this entry
 * @ptr:    the &struct slist_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_head within the struct.
 */
#define slist_entry(ptr, type, member) \
        container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define slist_first_entry(ptr, type, member) \
        slist_entry((ptr)->next, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 * 
 * Note that if the list is empty, it returns NULL.
 */
#define slist_first_entry_or_null(ptr, type, member) ({         \
    struct slist_head *head__ = (ptr);                          \
    struct slist_head *pos__ = head__->next;                    \
    pos__ != head__ ? slist_entry(pos__, type, member) : NULL;  \
})

/**
 * slist_next_entry - get the next element in slist
 * @pos:    the type * to cursor
 * @member: the name of the list_head within the struct.
 */
#define slist_next_entry(pos, member) \
        slist_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * slist_for_each - iterate over a slist
 * @head:   the head for your list.
 * @pos:    the &struct slist_head to use as a loop cursor.
 */    
#define slist_for_each(head, pos) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
    
/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for (pos = slist_first_entry(head, typeof(*pos), member); \
        &pos->member != NULL; \
        pos = slist_next_entry(pos, member))

#endif
