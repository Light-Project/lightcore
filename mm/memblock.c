/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "memblk: " fmt

#include <mm/memblock.h>
#include <mm/memmodel.h>
#include <printk.h>

#define MEMBLOCK_REGIONS 256
static struct memblock_region memblock_regions[MEMBLOCK_REGIONS];
static LIST_HEAD(memblock_list);

static inline struct memblock_region *memblock_node_alloc(void)
{
    for (int count = 0; count < MEMBLOCK_REGIONS; ++count)
        if(!memblock_regions[count].use) {
            memblock_regions[count].use = true;
            return &memblock_regions[count];
        }
    return NULL;
}

static inline void memblock_node_remove(struct memblock_region *region)
{
    list_del(&region->list);
    region->use = false;
}

static state memblock_insert(phys_addr_t addr, size_t size, enum memblock_type type)
{
    struct memblock_region *block, *next, *new, *tmp, *last = NULL;
    struct list_head *prev = NULL;

    if (!size)
        return -EINVAL;

    new = memblock_node_alloc();
    if (!new)
        return -ENOMEM;

    list_for_each_entry_safe(block, next, &memblock_list, list) {
        phys_addr_t end, block_end;
        end = addr + size;
        block_end = block->addr + block->size;

        if (addr <= block->addr && block_end <= end) {
            /* completely cover */
            memblock_node_remove(block);
            prev = block->list.prev;
        } else if (addr <= block->addr && block->addr < end) {
            /* front cover */
            if (type == block->type) {
                size += block_end - end;
                memblock_node_remove(block);
            } else {
                block->addr = end;
                block->size -= size;
                prev = block->list.prev;
            }
            break;
        } else if (addr < block_end && block_end <= end) {
            /* back cover */
            if (type == block->type) {
                addr = block->addr;
                size += addr - block->addr;
                memblock_node_remove(block);
            } else {
                block->size -= block_end - addr;
                prev = &block->list;
            }
        } else if (block->addr < addr && end < block_end) {
            /* middle cover */
            if (type != block->type) {
                tmp = memblock_node_alloc();
                if (!tmp)
                    goto free;

                block->size = addr - block->addr;
                prev = &block->list;

                tmp->addr = end;
                tmp->size = block_end - end;
                tmp->type = block->type;
                list_add_next(prev, &tmp->list);
            }
            break;
        }

        if (block->addr < addr)
            last = block;
    }

    /* init new node */
    new->addr = addr;
    new->size = size;
    new->type = type;

    /* insert new node */
    if (!prev) {
        if (last)
            prev = &last->list;
        else  /* first node */
            prev = &memblock_list;
    }
    list_add_next(prev, &new->list);

    /* merge the prev same attribute node */
    if (last) {
        tmp = list_prev_entry(new, list);
        if (type == tmp->type && (tmp->addr + tmp->size + 1) == addr) {
            new->addr = tmp->addr;
            new->size += tmp->size;
            list_del(&tmp->list);
        }
    }

    /* merge the next same attribute node */
    tmp = list_next_entry(new, list);
    if (type == tmp->type && (addr + size + 1) == tmp->addr) {
        new->size += tmp->size;
        list_del(&tmp->list);
    }

    return -ENOERR;

free:
    memblock_node_remove(new);
    return -ENOMEM;
}

static state memblock_delete(phys_addr_t addr, size_t size)
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
                           phys_addr_t min_addr,
                           phys_addr_t max_addr)
{
    struct memblock_region *tmp, *block = NULL;
    phys_addr_t align_start;

    list_for_each_entry(tmp, &memblock_list, list) {
        align_start = align_high(tmp->addr, align);

        if (tmp->type != MEMBLOCK_USABLE)
            continue;

        if (tmp->addr < min_addr || max_addr < tmp->addr)
            continue;

        /* Prevent overflow caused by too large address. */
        if (align_start + size < align_start ||
            align_start < tmp->addr)
            continue;

        if (align_start + size <= tmp->addr + tmp->size) {
            block = tmp;
            break;
        }
    } if (!block) {
        return 0;
    }

    memblock_insert(align_start, size, MEMBLOCK_ALLOCATED);
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

state memblock_add(phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;

    if (!size)
        return -EINVAL;

    pr_info("add: [%lx - %lx]\n", addr, end);
    return memblock_insert(addr, size, MEMBLOCK_USABLE);
}

state memblock_reserve(phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;

    if (!size)
        return -EINVAL;

    pr_info("reserve: [%lx - %lx]\n", addr, end);
    return memblock_insert(addr, size, MEMBLOCK_RESERVED);
}

state memblock_remove(phys_addr_t addr, size_t size)
{
    phys_addr_t end = addr + size - 1;

    if (!size)
        return -EINVAL;

    pr_info("remove: [%lx - %lx]\n", addr, end);
    return memblock_delete(addr, size);
}
