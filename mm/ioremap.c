/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <ioremap.h>
#include <kernel.h>
#include <list.h>
#include <refcount.h>
#include <mm.h>
#include <kmalloc.h>
#include <mm/vmem.h>
#include <mm/vmap.h>
#include <export.h>
#include <printk.h>

static struct kcache *ioremap_cache;
static LIST_HEAD(ioremap_list);

struct ioremap_node {
    struct list_head list;
    struct vm_area vm_area;
    phys_addr_t start;
    refcount_t count;
};

#define vm_to_ioremap(vm) \
    container_of(vm, struct ioremap_node, vm_area)

static inline struct ioremap_node *
ioremap_find(phys_addr_t start, phys_addr_t end)
{
    struct ioremap_node *node;

    list_for_each_entry (node, &ioremap_list, list) {
        phys_addr_t vm_end = node->start + node->vm_area.size;
        if (node->start <= start && end <= vm_end)
            return node;
    }

    return NULL;
}

static struct ioremap_node *ioremap_alloc(phys_addr_t pa, size_t size)
{
    struct ioremap_node *node;

    node = kzalloc(sizeof(*node), GFP_KERNEL);
    if (unlikely(!node))
        return NULL;

    if (vmem_area_alloc(&node->vm_area, size, PAGE_SIZE)) {
        kfree(node);
        return NULL;
    }

    pa = align_low(pa, PAGE_SIZE);
    vmap_range(&init_mm, pa, node->vm_area.addr, node->vm_area.size, 0);

    node->start = pa;
    list_add_prev(&ioremap_list, &node->list);
    return node;
}

void *ioremap(phys_addr_t pa, size_t size)
{
    struct ioremap_node *node;
    phys_addr_t offset;

    if (!size)
        return NULL;

    /* Get mmio form ram pad */
    if (pa >= CONFIG_RAM_BASE &&
        pa < CONFIG_HIGHMEM_OFFSET)
        return pa_to_io(pa);

	/* Page-align mappings */
    offset = pa & ~PAGE_MASK;
    pa &= PAGE_MASK;
    size = align_high(size, PAGE_SIZE);

    node = ioremap_find(pa, pa + size);
    if (node)
        goto got_io;

    node = ioremap_alloc(pa, size);
    if (unlikely(!node))
        return NULL;

got_io:
    refcount_inc(&node->count);
    return (void *)(node->vm_area.addr + offset);
}

void iounmap(void *block)
{
    size_t addr = (size_t)block;
    struct ioremap_node *node;
    struct vm_area *vm;

    if (addr < CONFIG_HIGHMAP_OFFSET)
        return;

    vm = vmem_area_find(addr);
    if (!vm) {
        pr_crit("unmap not found\n");
        return;
    }

    node = vm_to_ioremap(vm);
    if (!refcount_dec_test(&node->count))
        return;

    vmem_free(vm);
    list_del(&node->list);
    kfree(node);
}

void __init ioremap_init(void)
{
    ioremap_cache = kcache_create("ioremap",
        sizeof(struct ioremap_node), KCACHE_PANIC);
}

EXPORT_SYMBOL(ioremap);
EXPORT_SYMBOL(iounmap);
