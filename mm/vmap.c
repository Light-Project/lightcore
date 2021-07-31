/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com> 
 */

#define pr_fmt(fmt) "vmalloc: " fmt

#include <state.h>
#include <mm/kmem.h>
#include <mm/vmem.h>
#include <mm/page.h>

/**
 * vmap - mapping page arrays to contiguous virtual addresses.
 * @page: page array pointer
 * @page_nr: 
 */
void *vmap(struct page *page, int page_nr)
{
    struct vm_area *va;
    size_t size;

    if(region_is_himem(page->region_type))
        return NULL;

    size = page_nr << PAGE_SHIFT;

    /* Apply for a piece of virtual memory */
    va = vmem_area_alloc();

    arch_page_map(va->addr, size, page);

    va->page = page;
    va->page_nr = page_nr;

    return va->addr;
}

void *vunmap(const void *addr, int page_nr)
{
    int i;

    if(unlikely(!addr))
        return;

    vmem_area_find


    for (i = 0; i < area->nr_pages; i++) {
        page = va->page[count];
        page_free(page, 0);
    }

}

static state kmalloc_get_page(struct vm_area *va, gfp_t gfp)
{
    unsigned int count;
    struct page *page;

    for(count = 0; count < va->page_nr; ++count) {
        page = page_alloc(0, gfp);

        if (unlikely(!page))
            goto fail;

        va->page[count] = page;
    }

    return -ENOERR;

fail:
    pr_err("page allocation failure\n");

    return -ENOMEM;
}

void *vmalloc_node(size_t size, size_t align, gfp_t gfp)
{
    struct vm_area;


    vm_area = vmem_alloc(size, );

    kmalloc_get_page(vm_area);

}

static vfree_area

/**
 * vmalloc - allocate virtual continuous memory.
 * @size: allocation size
 */
void *vmalloc(size_t size)
{

}

void vfree(void *addr)
{

}
