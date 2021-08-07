/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "page: " fmt

#include <mm/page.h>
#include <mm/region.h>

static __always_inline 
void free_list_add(struct page_free *free, struct page *page)
{
    list_add_prev(&free->list, &page->free_list);
    free->free_nr++;
}

static __always_inline 
void free_list_del(struct page_free *free, struct page *page)
{
    list_del(&page->free_list);
    free->free_nr--;
}

static __always_inline
struct page *free_list_get_page(struct page_free *free)
{
    return list_first_entry_or_null(&free->list, struct page, free_list);
}

static inline void queue_division(struct page_free *free, struct page *page, 
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

static struct page *queue_smallest(struct region *region, int order)
{
    struct page_free *free;
    struct page *page;
    uint nd;

    for (nd = order; nd < PAGE_ORDER_MAX; ++nd) {
        free = &region->page_free[nd];

        page = free_list_get_page(free);
        if (!page)
            continue;

        free_list_del(free, page);
        queue_division(&region->page_free[0], page, order, nd);
        page->order = order;
        return page;
    }

    return NULL;
}

static inline size_t find_buddy(size_t page_nr, uint order)
{
	return page_nr ^ (1 << order);
}

static inline bool check_buddy(struct page *page, struct page *buddy)
{
    if (page->region_type != buddy->region_type)
        return false;

    if (page->order != buddy->order)
        return false;

    return true;
}

void queue_merge(struct region *region, struct page *page, uint order)
{
    size_t buddy_pnr, pnr = page_to_nr(page);
    struct page_free *free;
    struct page *buddy;

    for (; order < PAGE_ORDER_MAX; ++order) {
        size_t merge_pnr;

        buddy_pnr = find_buddy(pnr, order);
        buddy = page + (buddy_pnr - pnr);

        if(!check_buddy(page, buddy))
            goto finish;

        free = &region->page_free[order];
        free_list_del(free, buddy);
    
        merge_pnr = buddy_pnr & pnr;
		page = page + (merge_pnr - pnr);
        pnr = merge_pnr;
    }

finish:
    page->order = order;
    free = &region->page_free[order];
    free_list_add(free, page);
}

void page_add(struct region *region, struct page *page,
              uint order, uint order_nr)
{
    uint size = 1 << order;

    while(order_nr--) {
        free_list_add(&region->page_free[order], page);
        page += size;
    }
}

void *page_address(const struct page *page)
{
    /*
     * If it's a kernel direct mapping, we only need a 
     * simple transformation 
     */
    if(!region_is_himem(page->region_type))
        return page_to_va(page);

    return NULL;
}

struct page *page_alloc(uint order, gfp_t gfp)
{
    struct page *page;
    struct region *region;

    if(unlikely(order > PAGE_ORDER_MAX))
        return NULL;

    region = gfp_to_region(gfp);
    page = queue_smallest(region, order);

    return page;
}

void page_free(struct page *page, uint order)
{

}

