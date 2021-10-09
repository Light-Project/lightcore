/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum resource_type {
    RESOURCE_NONE,
    RESOURCE_PMIO,
    RESOURCE_MMIO,
    RESOURCE_MMIO64,
    RESOURCE_IRQ,
    RESOURCE_DMA,
};

struct resource {
    const char *name;   /* Resource name */
    resource_size_t start;
    resource_size_t size;
    uint8_t type;
    struct list_head list;
};

#define res_size(resource) \
    (resource->end - resource->start)

#endif  /* _RESOURCE_H_ */
