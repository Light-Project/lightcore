/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "memblk: " fmt

#include <mm/memblock.h>
#include <error.h>
#include <printk.h>

#define MEMBLOCK_REGIONS 256
static struct memblock_region memblock_regions[MEMBLOCK_REGIONS];
static LIST_HEAD(memblock_list);

#ifdef CONFIG_MEMBLK_DBG
# define memblk_debug(fmt, ...) pr_debug(fmt, ##__VA_ARGS__)
#else
# define memblk_debug(fmt, ...) pr_none(fmt, ##__VA_ARGS__)
#endif

static inline struct memblock_region *memblock_node_alloc(void)
{
    for (int count = 0; count < MEMBLOCK_REGIONS; ++count) {
        if (!memblock_regions[count].use) {
            memblock_regions[count].use = true;
            return &memblock_regions[count];
        }
    }

    return NULL;
}

static inline void memblock_node_remove(struct memblock_region *region)
{
    list_del(&region->list);
    region->use = false;
}

static struct memblock_region *
memblock_insert(phys_addr_t addr, size_t size, enum memblock_type type)
{
    struct memblock_region *block, *next, *new, *tmp, *last = NULL;
    struct list_head *prev = NULL;

    new = memblock_node_alloc();
    if (unlikely(!new))
        return ERR_PTR(-ENOMEM);

    list_for_each_entry_safe(block, next, &memblock_list, list) {
        phys_addr_t end, block_end;
        end = addr + size - 1;
        block_end = block->addr + block->size - 1;

        if (addr <= block->addr && block_end <= end) {
            /* completely cover */
            memblock_node_remove(block);
        } else if (addr <= block->addr && block->addr < end) {
            /* front cover */
            if (type == block->type) {
                size += block_end - end;
                memblock_node_remove(block);
            } else {
                block->addr = end + 1;
                block->size -= size;
            }
            break;
        } else if (addr < block_end && block_end <= end) {
            /* back cover */
            if (type == block->type) {
                addr = block->addr;
                size += addr - block->addr;
                prev = block->list.prev;
                memblock_node_remove(block);
            } else {
                block->size = addr - block->addr;
                prev = &block->list;
            }
        } else if (block->addr < addr && end < block_end) {
            /* middle cover */
            if (type == block->type) {
                addr = block->addr;
                size = block->size;
                memblock_node_remove(block);
            } else {
                tmp = memblock_node_alloc();
                if (unlikely(!tmp))
                    goto free;

                block->size = addr - block->addr;
                prev = &block->list;

                tmp->addr = end + 1;
                tmp->size = block_end - end;
                tmp->type = block->type;
                tmp->name = block->name;
                list_add(prev, &tmp->list);
            }
            break;
        }

        if (!prev && block->addr < addr)
            last = block;
    }

    /* init new node */
    new->addr = addr;
    new->size = size;
    new->type = type;

    /* insert new node */
    if (!prev) {
        if (likely(last))
            prev = &last->list;
        else  /* first node */
            prev = &memblock_list;
    }
    list_add(prev, &new->list);

#ifdef CONFIG_MEMBLK_MERGE
    /* merge the prev same attribute node */
    if (prev != &memblock_list) {
        tmp = list_prev_entry(new, list);
        if (type == tmp->type && tmp->addr + tmp->size == addr) {
            new->addr = tmp->addr;
            new->size += tmp->size;
            memblock_node_remove(tmp);
        }
    }

    /* merge the next same attribute node */
    tmp = list_next_entry(new, list);
    if (type == tmp->type && addr + size == tmp->addr) {
        new->size += tmp->size;
        memblock_node_remove(tmp);
    }
#endif

    return new;

free:
    memblock_node_remove(new);
    return ERR_PTR(-ENOMEM);
}

static state memblock_clear(phys_addr_t addr, size_t size)
{
    struct memblock_region *block, *tmp;

    list_for_each_entry(block, &memblock_list, list) {
        phys_addr_t end, block_end;
        end = addr + size;
        block_end = block->addr + block->size;

        if (addr < block->addr && block->addr < end) {
            block->addr = end;
            break;
        } else if (addr < block_end && block_end < end) {
            block->size = block_end - addr;
        } else if (addr <= block->addr && block_end <= end) {
            memblock_node_remove(block);
        } else if (block->addr < addr && end < block_end) {
            block->addr = end;
            tmp = memblock_node_alloc();
            if (!tmp)
                return -ENOMEM;
            tmp->addr = end;
            tmp->size = block_end - end;
            break;
        }
    }

    return -ENOERR;
}

phys_addr_t memblock_alloc(size_t size, size_t align,
            phys_addr_t min_addr, phys_addr_t max_addr)
{
    struct memblock_region *tmp, *block = NULL;
    phys_addr_t align_start;

    if (!size)
        return -EINVAL;

    list_for_each_entry(tmp, &memblock_list, list) {
        align_start = align_high(tmp->addr, align);

        if (tmp->type != MEMBLOCK_USABLE || tmp->addr < min_addr)
            continue;

        if (max_addr < tmp->addr)
            break;

        /* Prevent overflow caused by too large address. */
        if (align_start + size < align_start ||
            align_start < tmp->addr)
            continue;

        if (align_start + size <= tmp->addr + tmp->size) {
            block = tmp;
            break;
        }
    } if (unlikely(!block)) {
        return 0;
    }

    block = memblock_insert(align_start, size, MEMBLOCK_ALLOCATED);
    if (PTR_ERR(block)) {
        pr_crit("no free memblock slot found\n");
        return 0;
    }

    block->name = "assigned";
    return align_start;
}

void memblock_takeover(enum memblock_type type, bool (*fn)(struct memblock_region *))
{
    struct memblock_region *block;
    list_for_each_entry(block, &memblock_list, list) {
        if (block->type == type)
        if (fn(block))
            block->type = MEMBLOCK_TAKEOVER;
    }
}

state memblock_add(const char *name, phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;
    struct memblock_region *node;
    state ret;

    if (!size)
        return -EINVAL;

    addr = align_high(addr, MSIZE);
    size = align_low(size, MSIZE);

    memblk_debug("usable [%#018lx - %#018lx] %s\n", addr, end, name);
    node = memblock_insert(addr, size, MEMBLOCK_USABLE);

    if ((ret = PTR_ERR(node))) {
        pr_crit("no free memblock slot found\n");
        return ret;
    }

    node->name = name;
    return -ENOERR;
}

state memblock_reserve(const char *name, phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;
    struct memblock_region *node;
    state ret;

    if (!size)
        return -EINVAL;

    addr = align_low(addr, MSIZE);
    size = align_high(size, MSIZE);

    memblk_debug("reserve [%#018lx - %#018lx] %s\n", addr, end, name);
    node = memblock_insert(addr, size, MEMBLOCK_RESERVED);

    if ((ret = PTR_ERR(node))) {
        pr_crit("no free memblock slot found\n");
        return ret;
    }

    node->name = name;
    return -ENOERR;
}

state memblock_remove(phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;

    if (!size)
        return -EINVAL;

    memblk_debug("clear [%#018lx - %#018lx]\n", addr, end);
    return memblock_clear(addr, size);
}

void memblock_dump(void)
{
    struct memblock_region *node;

    pr_info("memoryblock layout:\n");

    list_for_each_entry(node, &memblock_list, list) {
        phys_addr_t end = node->addr + node->size - 1;
        pr_info("  [%#018lx-%#018lx] %s type: %d\n",
                node->addr, end, node->name, node->type);
    }
}
