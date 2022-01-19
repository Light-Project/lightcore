/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMBLOCK_H_
#define _MM_MEMBLOCK_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum memblock_type {
    MEMBLOCK_USABLE     = 0x00, /* Memory that can be used */
    MEMBLOCK_ALLOCATED  = 0x01, /* allocated by memblock */
    MEMBLOCK_TAKEOVER   = 0x02, /* buddy system takeover */
    MEMBLOCK_RESERVED   = 0x03, /* nonexistent memory */
};

struct memblock_region {
    const char *name;
    phys_addr_t addr;
    phys_addr_t size;
    uint8_t use:1;
    uint8_t type:4;
    struct list_head list;
};

extern void memtest(void);
extern state memblock_add(const char *name, phys_addr_t addr, phys_addr_t size);
extern state memblock_reserve(const char *name, phys_addr_t addr, phys_addr_t size);
extern state memblock_remove(phys_addr_t addr, size_t size);
extern phys_addr_t memblock_alloc(size_t size, size_t align, phys_addr_t min_addr, phys_addr_t max_addr);
extern void memblock_takeover(enum memblock_type type, bool (*fn)(struct memblock_region *));
extern void memblock_dump(void);

#endif /* _MM_MEMBLOCK_H_ */
