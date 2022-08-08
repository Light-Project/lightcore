/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RANGE_H_
#define _RANGE_H_

#include <list.h>

typedef struct range_node *(*range_alloc_t)(void *pdata);
typedef void (*range_free_t)(struct range_node *node, void *pdata);
typedef void (*range_copy_t)(struct range_node *dest, const struct range_node *src);

struct range_head {
    struct list_head nodes;
    range_alloc_t alloc;
    range_free_t free;
    range_copy_t copy;
    void *pdata;
};

struct range_node {
    struct list_head list;
    unsigned long start;
    unsigned long size;
    unsigned int type;
};

extern struct range_node *range_find(struct range_head *head, unsigned int type, unsigned long align, unsigned long start, unsigned long size);
extern struct range_node *range_insert(struct range_head *head, unsigned int type, unsigned long start, unsigned long size, bool expel, bool merge);
extern state range_subtract(struct range_head *head, unsigned long start, unsigned long size);

#endif  /* _RANGE_H_ */
