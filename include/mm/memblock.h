/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMBLOCK_H_
#define _MM_MEMBLOCK_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum memblock_type {
    MEMBLOCK_USABLE     = 0x00, /* Memory that can be used */
    MEMBLOCK_ALLOCATED  = 0x01, /* allocated by memblock */
    MEMBLOCK_TAKEOVER   = 0x01, /* buddy system takeover */
    MEMBLOCK_RESERVED   = 0x02, /* nonexistent memory */
};

struct memblock_region {
    phys_addr_t addr;
    phys_addr_t size;
    uint8_t use:1;
    uint8_t type:4;
    struct list_head list;
};

state memblock_add(phys_addr_t addr, phys_addr_t size);
state memblock_reserve(phys_addr_t addr, phys_addr_t size);
state memblock_remove(phys_addr_t addr, size_t size);
phys_addr_t memblock_alloc(size_t size, size_t align, phys_addr_t min_addr, phys_addr_t max_addr);
void memblock_takeover(enum memblock_type type, bool (*fn)(struct memblock_region *));

#endif /* _MM_MEMBLOCK_H_ */
