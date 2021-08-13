/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/page.h>
#include <mm/memmodel.h>
#include <mm/memblock.h>
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

struct region *pa_to_region(phys_addr_t pa)
{
#ifdef CONFIG_REGION_DMA
    if (pa < CONFIG_DMA_SIZE)
        return &region_map[REGION_DMA];
#endif
#ifdef CONFIG_REGION_DMA32
    if (pa < CONFIG_DMA32_SIZE)
        return &region_map[REGION_DMA32];
#endif
#ifdef CONFIG_REGION_HIMEM
    if (CONFIG_HIGHMAP_OFFSET < pa)
        return &region_map[REGION_HIMEM];
#endif
    return &region_map[REGION_NORMAL];
}

static inline bool region_block_add(struct memblock_region *blk)
{
    struct region *region = pa_to_region(blk->addr);
    struct page *page = pa_to_page(align_high(blk->addr, PAGE_SIZE));

    if (!(blk->size >> PAGE_SHIFT))
        return false;

    if (!page)
        panic("page map not sync");

    page_add(region, page, 0, blk->size >> PAGE_SHIFT);
    return true;
}

static inline void __init region_free_list_init(void)
{
    for(int region = 0; region < REGION_NR_MAX; ++region)
    for(int order = 0; order <= PAGE_ORDER_MAX; ++order)
        list_head_init(&region_map[region].page_free[order].list);
}

void __weak __init region_init()
{
    region_free_list_init();
    memblock_takeover(MEMBLOCK_USABLE, region_block_add);
}