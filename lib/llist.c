/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <llist.h>
#include <export.h>

bool llist_split(struct slist_head *head, struct slist_head *new, struct slist_head *end)
{
    struct slist_head *first;

    do first = end->next = READ_ONCE(head->next);
    while (cmpxchg(&head->next, first, new) != first);

    return !first;
}
EXPORT_SYMBOL(llist_split);

struct slist_head *llist_del(struct slist_head *head)
{
    struct slist_head *old, *next, *entry;

    for (entry = READ_ONCE(head->next); (old = entry);) {
        next = READ_ONCE(entry->next);
        entry = cmpxchg(&head->next, old, next);
        if (old == entry)
            break;
    }

    return entry;
}
EXPORT_SYMBOL(llist_del);
