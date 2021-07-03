/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Mapping fixed addresses to highmem
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <math.h>
#include <stddef.h>
#include <system.h>
#include <system/spinlock.h>
#include <rbtree.h>
#include <list.h>
#include <export.h>
#include <printk.h>

#include <mm/ioremap.h>
#include <mm/vmem.h>
#include <mm/kmem.h>

#include <asm/page.h>

static LIST_HEAD(ioremap_list);
// static spinlock_t lock;

struct ioremap {
    phys_addr_t start;
    phys_addr_t end;
    struct vm_area *vm_area;

    struct list_head list;
    unsigned int c; /* citations nr */
};

static struct ioremap *ioremap_find(phys_addr_t start, phys_addr_t end)
{
    struct ioremap *tmp, *io = NULL;

    list_for_each_entry(tmp, &ioremap_list, list) {
        if((tmp->start <= start) && (tmp->end >= end)) {
            io = tmp;
            break;
        }
    }
    
    return io;
}

static struct ioremap *ioremap_get(phys_addr_t pa, size_t size)
{
    struct vm_area *vm;
    struct ioremap *io;

    vm = vmem_alloc(size, GFP_HIGHMEM);
    if(!vm)
        return NULL;

    vm->phys_addr = pa;
    arch_page_map(vm, GFP_HIGHMEM);

    io = kzalloc(sizeof(*io), GFP_KERNEL);
    if(!io)
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
 
    /* Don't allow zero size */
    if(!size)
        return NULL;

    /*
     * Mappings have to be page-aligned
     */
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
    io->c++;

    return io->vm_area->addr + offset;
}
EXPORT_SYMBOL(ioremap);

void iounmap(void *addr)
{
    struct ioremap *tmp, *io;
    struct vm_area *va;

    if((size_t)addr <= CONFIG_HIGHMEM_OFFSET)
        return;

    list_for_each_entry(tmp, &ioremap_list, list) {
        va = tmp->vm_area;
        if((va->addr <= addr)&&(addr <= va->addr + va->size)) {
            io = tmp;
            break;
        }
    }

    kassert(io);

    
}
EXPORT_SYMBOL(iounmap);
