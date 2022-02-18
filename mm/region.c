/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/region.h>
#include <mm/memblock.h>
#include <mm/memmodel.h>
#include <mm/page.h>
#include <export.h>

struct region region_map[REGION_NR_MAX] = {
#ifdef CONFIG_REGION_DMA
    [REGION_DMA] = {
        .name = "DMA",
    },
#endif
#ifdef CONFIG_REGION_DMA32
    [REGION_DMA32] = {
        .name = "DMA32",
    },
#endif
    [REGION_NORMAL] = {
        .name = "Normal",
    },
#ifdef CONFIG_REGION_HIMEM
    [REGION_HIMEM] = {
        .name = "HighMem",
    },
#endif
};

enum region_type gfp_to_region(gfp_t gfp)
{
    switch (gfp & GFP_REGION_MASK) {
#ifdef CONFIG_REGION_DMA
        case GFP_DMA:
            return REGION_DMA;
#endif
#ifdef CONFIG_REGION_DMA32
        case GFP_DMA32:
            return REGION_DMA32;
#endif
#ifdef CONFIG_REGION_HIMEM
        case GFP_HIGHMEM:
            return REGION_HIMEM;
#endif
    }
    return REGION_NORMAL;
}
EXPORT_SYMBOL(gfp_to_region);

enum region_type pa_to_region(phys_addr_t pa)
{
#ifdef CONFIG_REGION_DMA
    if (pa < (CONFIG_RAM_BASE + CONFIG_DMA_SIZE))
        return REGION_DMA;
#endif
#ifdef CONFIG_REGION_DMA32
    if (pa < (CONFIG_RAM_BASE + CONFIG_DMA32_SIZE))
        return REGION_DMA32;
#endif
#ifdef CONFIG_REGION_HIMEM
    if (pa >= (CONFIG_RAM_BASE + CONFIG_HIGHMEM_OFFSET))
        return REGION_HIMEM;
#endif
    return REGION_NORMAL;
}
EXPORT_SYMBOL(pa_to_region);

enum region_type page_to_region(const struct page *page)
{
    return pa_to_region(page_to_pa(page));
}
EXPORT_SYMBOL(page_to_region);

static bool region_block_add(struct memblock_region *blk)
{
    enum region_type region;
    struct page *page;
    phys_addr_t addr, end;

    addr = align_high(blk->addr, PAGE_SIZE);
    end = align_low(blk->addr + blk->size, PAGE_SIZE);

    if (end - addr < PAGE_SIZE)
        return false;

    for (; addr < end; addr += PAGE_SIZE) {
        region = pa_to_region(addr);
        page = pa_to_page(addr);
        page_add(&region_map[region], page, 0, 1);
    }

    return true;
}

static void __init region_free_list_init(void)
{
    unsigned int region, order;
    for (region = 0; region < REGION_NR_MAX; ++region)
    for (order = 0; order <= PAGE_ORDER_MAX; ++order)
        list_head_init(&region_map[region].page_free[order].list);
}

void __init region_init()
{
    region_free_list_init();
    memblock_takeover(MEMBLOCK_USABLE, region_block_add);
}
