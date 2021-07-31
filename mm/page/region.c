/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/page.h>
#include <mm/memmodel.h>
#include <mm/region.h>

struct region region_map[REGION_NR_MAX];

struct region *gfp_to_region(gfp_t gfp)
{
    switch(gfp & GFP_REGION_MASK) {
#ifdef CONFIG_REGION_DMA
        case GFP_DMA:
            return &region_map[REGION_DMA];
#endif
#ifdef CONFIG_REGION_DMA32
        case GFP_DMA32:
            return &region_map[REGION_DMA32];
#endif
#ifdef CONFIG_REGION_HIMEM
        case GFP_HIGHMEM:
            return &region_map[REGION_HIMEM];
#endif
    }
    return &region_map[REGION_NORMAL];
}

static inline void __init region_free_list_init(void)
{
    unsigned int region, order;

    for(region = 0; region < REGION_NR_MAX; ++region)
    for(order = 0; order < PAGE_ORDER_MAX; ++order)
        list_head_init(&region_map[region].page_free[order].list);
}

void region_mem_add(enum region_type type, 
                    phys_addr_t addr, size_t size)
{
    struct region *region = &region_map[type] ;
    struct page *page = &pa_to_page(addr);

    page_add(region, page, PAGE_ORDER_MAX - 1, 
             order_nr(size, PAGE_ORDER_MAX));
}

void __weak __init region_init()
{
    phys_addr_t addr = CONFIG_RAM_BASE;

    region_free_list_init();

#ifdef CONFIG_REGION_DMA
    region_mem_add(REGION_DMA, addr, CONFIG_DMA_SIZE);
    addr += CONFIG_DMA_SIZE;
#endif
#ifdef CONFIG_REGION_DMA32
    region_mem_add(REGION_DMA32, addr, CONFIG_DMA32_SIZE);
    addr += CONFIG_DMA32_SIZE;
#endif
    region_mem_add(REGION_NORMAL, addr + 0x100000, CONFIG_HIGHMEM_OFFSET - addr);
#ifdef CONFIG_REGION_HIMEM
    region_mem_add(REGION_HIMEM, CONFIG_HIGHMEM_OFFSET, ~0 - CONFIG_HIGHMEM_OFFSET);
#endif
}