/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum res_type{
    RESOURCE_NONE   = 0x00,     /* None I/O */
    RESOURCE_PMIO   = 0x01,     /* Port-mapped I/O */
    RESOURCE_MMIO   = 0x02,     /* Memory-mapped I/O */
    RESOURCE_MMIO64 = 0x03,     /* Memory-mapped I/O */
};

struct resource {
    const char *name;   /* Resource name */
    struct list_head list;
    uint32_t type:4;
    resource_size_t start;
    resource_size_t end;
};

#endif  /* _RESOURCE_H_ */
