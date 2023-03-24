/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LLIST_H_
#define _LLIST_H_

#include <slist.h>
#include <atomic.h>

extern bool llist_split(struct slist_head *head, struct slist_head *new, struct slist_head *end);
extern struct slist_head *llist_del(struct slist_head *head);

static inline bool llist_add(struct slist_head *head, struct slist_head *new)
{
    return llist_split(head, new, new);
}

static inline struct slist_head *llist_destroy(struct slist_head *head)
{
    return xchg(&head->next, NULL);
}

#endif /* _LLIST_H_ */
