/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "memblk"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <mm/memblock.h>
#include <string.h>
#include <error.h>
#include <printk.h>

#define MEMBLOCK_REGIONS CONFIG_MEMBLK_REGIONS
static bool memblock_merge = IS_ENABLED(CONFIG_MEMBLK_MERGE);
static struct memblock_region memblock_regions[MEMBLOCK_REGIONS];

static __init struct range_node *memblock_node_alloc(void *pdata);
static __init void memblock_node_remove(struct range_node *range, void *pdata);
static __init void memblock_node_copy(struct range_node *dest, const struct range_node *src);
static DEFINE_RANGE(memblock_range, memblock_node_alloc, memblock_node_remove, memblock_node_copy, NULL);

#ifdef CONFIG_DEBUG_MEMBLOCK
# define memblk_debug(fmt, ...) pr_debug(fmt, ##__VA_ARGS__)
#else
# define memblk_debug(fmt, ...) pr_none(fmt, ##__VA_ARGS__)
#endif

#ifdef CONFIG_NUMA
static inline void memblock_node_set_numa(struct memblock_region *reg, int nid)
{
    reg->nid = nid;
}

static inline int memblock_node_get_numa(struct memblock_region *reg)
{
    return reg->nid;
}
#else
static inline void memblock_node_set_numa(struct memblock_region *reg, int nid)
{
    /* Nothing */
}

static inline int memblock_node_get_numa(struct memblock_region *reg)
{
    return 0;
}
#endif

static __init struct range_node *memblock_node_alloc(void *pdata)
{
    for (int count = 0; count < MEMBLOCK_REGIONS; ++count) {
        if (!memblock_regions[count].used) {
            memblock_regions[count].used = true;
            return &memblock_regions[count].range;
        }
    }

    return NULL;
}

static __init void
memblock_node_remove(struct range_node *range, void *pdata)
{
    struct memblock_region *region = range_to_memblock(range);
    region->used = false;
}

static __init void
memblock_node_copy(struct range_node *dest, const struct range_node *src)
{
    struct memblock_region *rdest = range_to_memblock(dest);
    const struct memblock_region *rsrc = range_to_memblock(src);
    rdest->name = rsrc->name;
}

static __init struct memblock_region *
memblock_insert(phys_addr_t addr, size_t size,
                enum memblock_type type, bool merge)
{
    struct range_node *range;
    range = range_insert(&memblock_range, type, addr, size,
                         RANGE_DISLODGE, merge);
    return range_to_memblock(range);
}

static __init state
memblock_clear(phys_addr_t addr, size_t size)
{
    return range_subtract(&memblock_range, addr, size);
}

phys_addr_t __init
memblock_phys_alloc_range(size_t size, size_t align, phys_addr_t min_addr,
                          phys_addr_t max_addr, int numa)
{
    struct memblock_region *tmp, *block = NULL;
    phys_addr_t align_start;

    if (unlikely(!size))
        return 0;

    list_for_each_entry(tmp, &memblock_range.nodes, range.list) {
        align_start = align_high(tmp->range.start, align);

        if (tmp->range.type != MEMBLOCK_USABLE ||
            tmp->range.start < min_addr)
            continue;

        if (max_addr < tmp->range.start)
            break;

        /* Prevent overflow caused by too large address. */
        if (align_start + size < align_start ||
            align_start < tmp->range.start)
            continue;

        if (align_start + size <= tmp->range.start +
            tmp->range.size) {
            block = tmp;
            break;
        }
    }

    if (unlikely(!block))
        return 0;

    block = memblock_insert(align_start, size, MEMBLOCK_ALLOCATED, true);
    if (unlikely(IS_INVAL(block))) {
        pr_crit("No free memblock slot found.\n");
        return 0;
    }

    block->name = "assigned";
    return align_start;
}

static void __init *memblock_alloc_internal(size_t size, size_t align, phys_addr_t min_addr,
                                            phys_addr_t max_addr, int numa)
{
    phys_addr_t phys;

    phys = memblock_phys_alloc_range(size, align, min_addr, max_addr, numa);
    if (!phys && min_addr)
        phys = memblock_phys_alloc_range(size, align, 0, max_addr, numa);

    if (!phys)
        return NULL;

    return pa_to_va(phys);
}

void __init *memblock_alloc_range(size_t size, size_t align, phys_addr_t min_addr,
                                  phys_addr_t max_addr, int numa)
{
    void *block;

    block = memblock_alloc_internal(size, align, min_addr, max_addr, numa);
    if (!block)
        return NULL;

    memset(block, 0, size);
    return block;
}

void __init memblock_takeover(enum memblock_type type, bool (*fn)(struct memblock_region *))
{
    struct memblock_region *block;

    list_for_each_entry(block, &memblock_range.nodes, range.list) {
        if (block->range.type == type)
        if (fn(block))
            block->range.type = MEMBLOCK_TAKEOVER;
    }
}

state __init memblock_add_numa(const char *name, phys_addr_t addr, size_t size, int numa)
{
    phys_addr_t end = addr + size - 1;
    struct memblock_region *node;

    if (unlikely(!size))
        return -EINVAL;

    addr = align_high(addr, MSIZE);
    size = align_low(size, MSIZE);

    memblk_debug("usable [%#018lx - %#018lx] %s\n", addr, end, name);
    node = memblock_insert(addr, size, MEMBLOCK_USABLE, memblock_merge);

    if (unlikely(IS_INVAL(node))) {
        pr_crit("no free memblock slot found.\n");
        return PTR_INVAL(node);
    }

    node->name = name;
    memblock_node_set_numa(node, numa);

    return -ENOERR;
}

state __init memblock_reserve(const char *name, phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;
    struct memblock_region *node;

    if (unlikely(!size))
        return -EINVAL;

    addr = align_low(addr, MSIZE);
    size = align_high(size, MSIZE);

    memblk_debug("reserve [%#018lx - %#018lx] %s\n", addr, end, name);
    node = memblock_insert(addr, size, MEMBLOCK_RESERVED, memblock_merge);

    if (unlikely(IS_INVAL(node))) {
        pr_crit("no free memblock slot found\n");
        return PTR_INVAL(node);
    }

    node->name = name;
    memblock_node_set_numa(node, 0);

    return -ENOERR;
}

state __init memblock_remove(phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;

    if (unlikely(!size))
        return -EINVAL;

    memblk_debug("clear [%#018lx - %#018lx]\n", addr, end);
    return memblock_clear(addr, size);
}

void __init memblock_dump(void)
{
    struct memblock_region *node;

    pr_info("memblock layout:\n");
    list_for_each_entry(node, &memblock_range.nodes, range.list) {
        phys_addr_t end = node->range.start + node->range.size - 1;
        pr_info("  [%#018lx-%#018lx] %s type: %d\n",
                node->range.start, end, node->name, node->range.type);
    }
}
