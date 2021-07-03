#ifndef _SLIST_H_
#define _SLIST_H_

#include <stddef.h>
#include <types.h>

typedef struct slist_head{
    struct slist_head *next;
} slist_t;

#define SLIST_HEAD(name)\
    struct slist_head name = { &name }
    
/**
 * slist_entry - get the struct for this entry
 * @ptr:    the &struct slist_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_head within the struct.
 */
#define slist_entry(ptr, type, member) \
        container_of(ptr, type, member)

#define slist_first_entry(ptr, type, member) \
        slist_entry((ptr)->next, type, member)

/**
 * slist_next_entry - get the next element in slist
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define slist_next_entry(pos, member) \
        slist_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * slist_for_each - iterate over a slist
 * @head: the head for your list.
 * @pos:  the &struct slist_head to use as a loop cursor.
 */    
#define slist_for_each(head, pos) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
    
/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for (pos = slist_first_entry(head, typeof(*pos), member); \
        &pos->member != head; \
        pos = slist_next_entry(pos, member))

void slist_head_init(struct slist_head *list);

void slist_add(struct slist_head *node, struct slist_head *new);

void slist_del(struct slist_head *head, struct slist_head *node);

void slist_replace(struct slist_head *head, struct slist_head *old, struct slist_head *new);

bool slist_check_first(struct slist_head *head, struct slist_head *node);
bool slist_check_end(struct slist_head *node);
bool slist_check_empty(struct slist_head *head);

#endif
