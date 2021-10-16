/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <kernel.h>
#include <list.h>
#include <mm.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <mm/vmem.h>
#include <export.h>
#include <printk.h>

#include <asm/atomic.h>
#include <asm/pgtable.h>

static LIST_HEAD(ioremap_list);

struct ioremap_node {
    struct list_head list;
    struct vm_area vm_area;
    phys_addr_t start;
    atomic_t count;
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

static struct ioremap_node *remap_node(phys_addr_t pa, size_t size)
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
    arch_page_map(pa, node->vm_area.addr, node->vm_area.size);

    node->start = pa;
    list_add_prev(&ioremap_list, &node->list);
    return node;
}

static void unmap_node(size_t addr)
{
    struct ioremap_node *node;
    struct vm_area *vm;

    return;

    if (addr < CONFIG_HIGHMAP_OFFSET)
        return;

    vm = vmem_area_find(addr);
    if (!vm) {
        pr_err("unmap not found\n");
        return;
    }

    node = vm_to_ioremap(vm);

    if (atomic_dec_and_test(&node->count)) {
        vmem_free(vm);
        list_del(&node->list);
        kfree(node);
    }
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
        return pa_to_va(pa);

	/* Page-align mappings */
    offset = pa & ~PAGE_MASK;
    pa &= PAGE_MASK;
    size = align_high(size, PAGE_SIZE);

    node = ioremap_find(pa, pa + size);
    if (node)
        goto got_io;

    node = remap_node(pa, size);
    if (!node)
        return NULL;

got_io:
    atomic_inc(&node->count);
    return (void *)(node->vm_area.addr + offset);
}
EXPORT_SYMBOL(ioremap);


void iounmap(void *addr)
{
    unmap_node((size_t)addr);
}
EXPORT_SYMBOL(iounmap);
