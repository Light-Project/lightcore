/* SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * The buddy system is used to manage the physical 
 * memory of the system, the virtual memory is 
 * managed by its own parts
 * 
 */

#include <stddef.h>
#include <mm/page.h>

#define PAGE_BITMAP_NR  (PAGE_NR / MSIZE)
struct page page_map[PAGE_NR];
size_t page_bitmap[PAGE_BITMAP_NR];

/**
 * page_to_address - get the virtual address of page
 * 
 */
void *page_to_address(const struct page *page)
{
    /*
     * If it's a kernel direct mapping, we only need a 
     * simple transformation 
     */
    if(!region_is_himem(page->region_type))
        return page_to_va(page);



    return NULL;
}

/**
 * page_alloc - alloc page of namber 2^nr
 */
struct page *page_alloc(unsigned int order, gfp_t gfp)
{
    struct vm_area *va;
    struct page *page;

    /* Get page through buddy system */
    page = buddy_alloc(order, gfp);

    if(gfp & GFP_HIGHMEM) {
        va = vmem_alloc(2 ^ order, gfp);
        va->page = &page;
    }

    arch_page_map(va, gfp);

    return page;
}


void page_free(struct page *page, unsigned int order)
{
    
    
}

static void __init page_free_lists_init()
{
    unsigned int region, order;

    for(region = 0; region < REGION_NR_MAX; ++region)
    for(order = 0; order < PAGE_ORDER_MAX; ++order)
        list_head_init(&region_map[region].page_free[order].list);
}

#include <size.h>
#define msize (512 * size_1MiB)

void __init page_init()
{
    phys_addr_t addr = 0;

    page_free_lists_init();

#ifdef CONFIG_REGION_DMA
    buddy_add(&region_map[REGION_DMA], CONFIG_DMA_SIZE >> PAGE_ORDER_MAX, 
              PAGE_ORDER_MAX - 1, &pa_to_page(addr));
    addr += CONFIG_DMA_SIZE;
#endif
#ifdef CONFIG_REGION_DMA32
    buddy_add(&region_map[REGION_DMA], CONFIG_DMA32_SIZE >> PAGE_ORDER_MAX, 
              PAGE_ORDER_MAX - 1, &pa_to_page(addr));
    addr += CONFIG_DMA32_SIZE;
    size -= CONFIG_DMA32_SIZE;
#endif
    buddy_add(&region_map[REGION_NORMAL], CONFIG_HIGHMEM_OFFSET >> PAGE_ORDER_MAX, 
              PAGE_ORDER_MAX - 1, &pa_to_page(addr));
    addr += CONFIG_HIGHMEM_OFFSET >> PAGE_ORDER_MAX;
#ifdef CONFIG_REGION_HIMEM
    buddy_add(&region_map[REGION_HIMEM], (msize - CONFIG_HIGHMEM_OFFSET) >> PAGE_ORDER_MAX, 
              PAGE_ORDER_MAX - 1, &pa_to_page(addr));
#endif
}
