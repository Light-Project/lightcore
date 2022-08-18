/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <list.h>

typedef long (*skiplist_find_t)(const void *data, const void *key);
typedef long (*skiplist_cmp_t)(const void *dataa, const void *datab);
typedef unsigned long (*skiplist_even_find_t)(const void *data, const void *key, bool *signplus);
typedef unsigned long (*skiplist_even_cmp_t)(const void *dataa, const void *datab, bool *signplus);

struct skip_node {
    void *pdata;
    struct list_head list[0];
};

struct skip_head {
    unsigned int count;
    unsigned int curr;
    unsigned int levels;
    gfp_t flags;
    struct list_head nodes[0];
};

extern state skiplist_insert(struct skip_head *head, void *data, skiplist_cmp_t cmp);
extern void skiplist_delete(struct skip_head *head, void *key, skiplist_find_t find);
extern void *skiplist_find(struct skip_head *head, void *key, skiplist_find_t find);

extern state skiplist_even_insert(struct skip_head *head, void *data, skiplist_even_cmp_t cmp);
extern void skiplist_even_delete(struct skip_head *head, void *key, skiplist_even_find_t find);
extern void *skiplist_even_find(struct skip_head *head, void *key, skiplist_even_find_t find);

extern void skiplist_reset(struct skip_head *head);
extern struct skip_head *skiplist_create(unsigned int levels, gfp_t flags);
extern void skiplist_destroy(struct skip_head *head);

#endif  /* _SKIPLIST_H_ */
