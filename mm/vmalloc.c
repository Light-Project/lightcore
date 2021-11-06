/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "vmalloc: " fmt

#include <vmalloc.h>
#include <numa.h>
#include <mm.h>
#include <mm/vmap.h>
#include <mm/page.h>
#include <mm/region.h>
#include <kmalloc.h>

struct vmalloc_area {
    struct vm_area vmem;
    struct page **page;
    unsigned long page_nr;
};

#define vmem_to_vmalloc(vmp) \
    container_of(vmp, struct vmalloc_area, vmem)

static struct kcache *vmalloc_cache;

static void free_pages(struct page **page_area, unsigned long page_nr)
{
    unsigned long count;
    struct page *page;

    for (count = 0; count < page_nr; ++count) {
        page = page_area[count];
        page_free(page);
    }
}

static state get_pages(struct vmalloc_area *area, gfp_t gfp)
{
    unsigned int count;
    struct page *page;

    for (count = 0; count < area->page_nr; ++count) {
        page = page_alloc(0, gfp);
        if (unlikely(!page))
            goto fail;
        area->page[count] = page;
    }

    return -ENOERR;

fail:
    free_pages(area->page, ++count);
    pr_err("page allocation failure\n");
    return -ENOMEM;
}

/**
 * vmalloc_page - mapping page arrays to contiguous virtual addresses.
 * @page: page array pointer
 */
void *vmalloc_page(struct page *page)
{
    struct vmalloc_area *vmalloc;
    size_t size = PAGE_SHIFT << page->order;

    if (page->region_type != REGION_HIMEM)
        return page_to_va(page);

    vmalloc = kcache_zalloc(vmalloc_cache, GFP_KERNEL);
    if (unlikely(!vmalloc))
        return NULL;

    vmalloc->page_nr = 1;
    vmalloc->page[0] = page;

    /* Apply for a piece of virtual memory */
    if (vmem_area_alloc(&vmalloc->vmem, size, PAGE_SIZE))
        goto err_free;

    if (vmap_pages(&init_mm, &page, 1, vmalloc->vmem.addr, 0))
        goto err_vmem;

    return (void *)vmalloc->vmem.addr;

err_vmem:
    vmem_area_free(&vmalloc->vmem);
err_free:
    kcache_free(vmalloc_cache, vmalloc);
    return NULL;
}

void *vmalloc_node(size_t size, size_t align, gfp_t gfp, int numa)
{
    struct vmalloc_area *vmalloc;
    struct page **page_area;
    unsigned long page_nr;

    vmalloc = kcache_zalloc(vmalloc_cache, GFP_KERNEL);
    if (unlikely(!vmalloc))
        return NULL;

    page_nr = size >> PAGE_SHIFT;
    page_area = kmalloc(sizeof(*page_area) * page_nr, GFP_KERNEL);
    if (unlikely(!vmalloc))
        goto err_vmalloc;

    if (vmem_area_alloc(&vmalloc->vmem, size, PAGE_SIZE))
        goto err_area;

    vmalloc->page_nr = page_nr;
    if (get_pages(vmalloc, gfp))
        goto err_vmem;

    if (vmap_pages(&init_mm, page_area, page_nr, vmalloc->vmem.addr, 0))
        goto err_pages;

    return (void *)vmalloc->vmem.addr;

err_pages:
    free_pages(page_area, page_nr);
err_vmem:
    vmem_area_free(&vmalloc->vmem);
err_area:
    kfree(page_area);
err_vmalloc:
    kcache_free(vmalloc_cache, vmalloc);
    return NULL;
}

void *vmalloc(size_t size)
{
    return vmalloc_node(size, MSIZE, GFP_KERNEL, NUMA_NONE);
}

void *vzalloc(size_t size)
{
    return vmalloc_node(size, MSIZE, GFP_KERNEL | GFP_ZERO, NUMA_NONE);
}

void vfree(void *addr)
{
    struct vmalloc_area *vmalloc;
    struct vm_area *vmem;

    if ((size_t)addr < CONFIG_HIGHMAP_OFFSET)
        return;

    vmem = vmem_area_find((size_t)addr);
    if (!vmem) {
        pr_err("unmap not found\n");
        return;
    }

    vmalloc = vmem_to_vmalloc(vmem);

    free_pages(vmalloc->page, vmalloc->page_nr);
    vmem_free(&vmalloc->vmem);
    kcache_free(vmalloc_cache, vmalloc);
}

void __init vmalloc_init(void)
{
    vmalloc_cache = kcache_create("kmalloc",
        sizeof(struct vmalloc_area), KCACHE_PANIC);
}
