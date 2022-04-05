/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <bits.h>
#include <list.h>
#include <rbtree.h>

enum resource_type {
    /* Resource types */
    RESOURCE_NONE       = BIT_SHIFT(0, 0),
    RESOURCE_PMIO       = BIT_SHIFT(0, 1),
    RESOURCE_MMIO       = BIT_SHIFT(0, 2),
    RESOURCE_IRQ        = BIT_SHIFT(0, 3),
    RESOURCE_DMA        = BIT_SHIFT(0, 4),
    RESOURCE_TYPE       = BIT_RANGE(7, 0),

    /* Resource extended flags */
    RESOURCE_MEM64      = BIT_SHIFT(8, 1),
    RESOURCE_DISABLED   = BIT_SHIFT(8, 2),
    RESOURCE_UNSET      = BIT_SHIFT(8, 4),
    RESOURCE_BUSY       = BIT_SHIFT(8, 8),

    /* PCI Resource */
    RESOURCE_PCI_ROM_ENABLE = BIT_SHIFT(16, 1),
    RESOURCE_PCI_ROM_SHADOW = BIT_SHIFT(16, 2),
};

struct resource {
    struct list_head siblings;
    struct list_head child;
    struct resource *parent;
    struct rb_node sort;
    const char *name;
    resource_size_t start;
    resource_size_t size;
    enum resource_type type;
};

#define rb_to_resource(io) \
    rb_entry(io, struct resource, sort)

#define rb_to_resource_safe(io) \
    rb_entry_safe(io, struct resource, sort)

static inline resource_size_t resource_end(const struct resource *resource)
{
    return (resource->start + resource->size) - 1;
}

static inline enum resource_type resource_type(const struct resource *resource)
{
    return resource->type & RESOURCE_TYPE;
}

extern struct resource *resource_register_conflict(struct resource *parent, struct resource *res);
extern state resource_register(struct resource *parent, struct resource *res);
extern state resource_unregister(struct resource *res);

#endif  /* _RESOURCE_H_ */
