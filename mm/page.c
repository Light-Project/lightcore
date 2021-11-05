/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "page: " fmt

#include <string.h>
#include <mm.h>
#include <mm/region.h>
#include <mm/page.h>

static __always_inline void free_list_add(struct page_free *free, struct page *page)
{
    list_add_prev(&free->list, &page->free_list);
    free->free_nr++;
}

static inline void free_list_del(struct page_free *free, struct page *page)
{
    list_del(&page->free_list);
    free->free_nr--;
}

static __always_inline
struct page *free_list_get_page(struct page_free *free)
{
    return list_first_entry_or_null(&free->list, struct page, free_list);
}

static inline void buddy_division(struct page_free *free, struct page *page,
                                  int need_order, int alloc_order)
{
    size_t size = 1 << alloc_order;

    while (need_order < alloc_order--) {
		size >>= 1;

        /* Put buddy into free list */
        free_list_add(&free[alloc_order], &page[size]);
        page[size].order = alloc_order;
    }
}

static struct page *buddy_smallest(struct region *region, int order)
{
    struct page_free *free;
    struct page *page;
    uint nd;

    for (nd = order; nd <= PAGE_ORDER_MAX; ++nd) {
        free = &region->page_free[nd];

        page = free_list_get_page(free);
        if (!page)
            continue;

        free_list_del(free, page);
        buddy_division(&region->page_free[0], page, order, nd);
        page->order = order;
        return page;
    }

    return NULL;
}

static inline size_t find_buddy(size_t page_nr, uint order)
{
	return page_nr ^ (1 << order);
}

static inline bool check_buddy(struct page *page, unsigned int order,
                               struct page *buddy)
{
    if (buddy->type != PAGE_FREE)
        return false;

    if (page->region_type != buddy->region_type)
        return false;

    if (order != buddy->order)
        return false;

    return true;
}

void buddy_merge(struct region *region, struct page *page, uint order)
{
    size_t buddy_pnr, pnr = page_to_nr(page);
    struct page_free *free;
    struct page *buddy;

    for (; order < PAGE_ORDER_MAX; ++order) {
        size_t merge_pnr;

        buddy_pnr = find_buddy(pnr, order);
        buddy = page + (buddy_pnr - pnr);

        if (!check_buddy(page, order, buddy))
            goto finish;

        free = &region->page_free[order];
        free_list_del(free, buddy);

        merge_pnr = buddy_pnr & pnr;
		page = page + (merge_pnr - pnr);
        pnr = merge_pnr;
    }

finish:
    page->type = PAGE_FREE;
    page->order = order;
    free = &region->page_free[order];
    free_list_add(free, page);
}

struct page *page_alloc(uint order, gfp_t gfp)
{
    struct page *page;
    struct region *region;

    if(unlikely(order > PAGE_ORDER_MAX))
        return NULL;

    region = gfp_to_region(gfp);

    page = buddy_smallest(region, order);
    if (unlikely(!page))
        return NULL;

    if (gfp & GFP_ZERO)
        memset(page_to_va(page), 0, PAGE_SIZE << order);
    return page;
}

void page_free(struct page *page)
{
    phys_addr_t pa = page_to_pa(page);
    struct region *region = pa_to_region(pa);
    buddy_merge(region, page, page->order);
}

void page_add(struct region *region, struct page *page,
              unsigned int order, unsigned int nr)
{
    uint size = 1 << order;
    for (; page && nr--; page += size)
        buddy_merge(region, page, order);
}
