/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <stddef.h>
#include <kernel.h>
#include <spinlock.h>
#include <rbtree.h>
#include <list.h>
#include <mm.h>
#include <mm/ioremap.h>
#include <export.h>
#include <printk.h>

#include <asm/pgtable.h>

static LIST_HEAD(ioremap_list);

struct ioremap {
    phys_addr_t start;
    phys_addr_t end;
    struct list_head list;
    struct vm_area *vm_area;
    int count;
};

static struct ioremap *ioremap_find(phys_addr_t start, phys_addr_t end)
{
    struct ioremap *tmp, *io = NULL;
    list_for_each_entry(tmp, &ioremap_list, list)
        if((tmp->start <= start) && (tmp->end >= end)) {
            io = tmp;
            break;
        }
    return io;
}

static struct ioremap *ioremap_get(phys_addr_t pa, size_t size)
{
    struct vm_area *vm;
    struct ioremap *io;

    vm = vmem_alloc(size);
    if (!vm)
        return NULL;

    pa = align_low(pa, PAGE_SIZE);
    arch_page_map(pa, vm->addr, vm->size);

    /* set the io node assigned to */
    io = kzalloc(sizeof(*io), GFP_KERNEL);
    if(unlikely(!io))
        vmem_free(vm);

    io->start = pa;
    io->end = pa + size;
    io->vm_area = vm;

    list_add_prev(&ioremap_list, &io->list);

    return io;
}

void *ioremap(phys_addr_t pa, size_t size)
{
    struct ioremap *io;
    phys_addr_t offset;

    if (!size)
        return NULL;

    if (pa >= CONFIG_RAM_BASE &&
        pa < CONFIG_HIGHMEM_OFFSET)
        return pa_to_va(pa);

    offset = pa & ~PAGE_MASK;
    pa &= PAGE_MASK;
    size = align_high(size, PAGE_SIZE);

    io = ioremap_find(pa, pa + size);
    if(io)
        goto got_io;

    io = ioremap_get(pa, size);
    if(!io)
        return NULL;

got_io:
    io->count++;
    return (void *)(io->vm_area->addr + offset);
}
EXPORT_SYMBOL(ioremap);

void iounmap(void *addr)
{
    struct ioremap *tmp, *io;
    struct vm_area *va;

    if (addr >= (void *)CONFIG_PAGE_OFFSET &&
        addr < (void *)CONFIG_HIGHMAP_OFFSET)
        return;

    list_for_each_entry(tmp, &ioremap_list, list) {
        va = tmp->vm_area;
        if((va->addr <= (size_t)addr)&&((size_t)addr <= va->addr + va->size)) {
            io = tmp;
            break;
        }
    }

    if (!--io->count)
        list_del(&io->list);
}
EXPORT_SYMBOL(iounmap);
