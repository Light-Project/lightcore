/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_MEDIAN_H_
#define _FILTER_MEDIAN_H_

#include <heap.h>
#include <mm/gfp.h>

struct median_node {
    struct heap_node node;
    bool minmax;
    int32_t value;
};

struct median_state {
    unsigned int count;
    unsigned int mask;
    struct heap_root minheap;
    struct heap_root maxheap;
    struct median_node nodes[0];
};

#define heap_to_median(ptr) \
    container_of(ptr, struct median_node, node)

extern int32_t median_update(struct median_state *state, int32_t value);
extern void median_clear(struct median_state *state);
extern struct median_state *median_alloc(unsigned int length, gfp_t flags);
extern void median_free(struct median_state *state);

#endif /* _FILTER_MEDIAN_H_ */
