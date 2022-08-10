/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RANGE_H_
#define _RANGE_H_

#include <list.h>

typedef struct range_node *(*range_alloc_t)(void *pdata);
typedef void (*range_free_t)(struct range_node *node, void *pdata);
typedef void (*range_copy_t)(struct range_node *dest, const struct range_node *src);

enum range_resize {
    RANGE_COLLISION     = 0,
    RANGE_DISLODGE      = 1,
    RANGE_RETRACTION    = 2,
};

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

#define RANGE_STATIC(name, _alloc, _free, _copy, _pdata) {  \
    .nodes = LIST_HEAD_STATIC((name).nodes),                \
    .alloc = _alloc, .free = _free,                         \
    .copy = _copy, .pdata = _pdata,                         \
}

#define RANGE_INIT(name, alloc, free, copy, pdata) \
    (struct range_head) RANGE_STATIC(name, alloc, free, copy, pdata)

#define DEFINE_RANGE(name, alloc, free, copy, pdata) \
    struct range_head name = RANGE_INIT(name, alloc, free, copy, pdata)

static inline void range_init(struct range_head *head, range_alloc_t alloc,
                              range_free_t free, range_copy_t copy, void *pdata)
{
    *head = RANGE_INIT(*head, alloc, free, copy, pdata);
}

extern struct range_node *range_insert(struct range_head *head, unsigned int type, unsigned long start, unsigned long size, enum range_resize resize, bool merge);
extern state range_subtract(struct range_head *head, unsigned long start, unsigned long size);
extern void range_release(struct range_head *head);

#endif  /* _RANGE_H_ */
