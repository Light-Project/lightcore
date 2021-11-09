/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <bits.h>

enum resource_type {
    RESOURCE_NONE       = BIT_SHIFT(0, 0),
    RESOURCE_PMIO       = BIT_SHIFT(0, 1),
    RESOURCE_MMIO       = BIT_SHIFT(0, 2),
    RESOURCE_IRQ        = BIT_SHIFT(0, 3),
    RESOURCE_DMA        = BIT_SHIFT(0, 4),
    RESOURCE_TYPE       = BIT_RANGE(7, 0),

    RESOURCE_MEM64      = BIT_SHIFT(8, 1),
};

struct resource {
    const char *name;
    struct list_head list;

    resource_size_t start;
    resource_size_t size;
    enum resource_type type;
};

static inline resource_size_t resource_end(struct resource *resource)
{
    return (resource->start + resource->size) - 1;
}

static inline enum resource_type resource_type(struct resource *resource)
{
    return resource->type & RESOURCE_TYPE;
}

#endif  /* _RESOURCE_H_ */
