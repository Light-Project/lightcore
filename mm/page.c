/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "page"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <mm/page.h>
#include <mm/region.h>
#include <string.h>
#include <export.h>

static __always_inline void
free_list_add(struct page_free *free, struct page *page)
{
    list_add_prev(&free->list, &page->free_list);
    free->free_nr++;
}

static __always_inline void
free_list_del(struct page_free *free, struct page *page)
{
    list_del(&page->free_list);
    free->free_nr--;
}

static __always_inline struct page *
free_list_get_page(struct page_free *free)
{
    return list_first_entry_or_null(&free->list, struct page, free_list);
}

static __always_inline unsigned long
find_buddy(unsigned long page_nr, unsigned int order)
{
    return page_nr ^ (1 << order);
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
    unsigned int nd;

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

static struct page *buddy_alloc(struct region *region, int order, gfp_t gfp)
{
    irqflags_t irqflags;
    struct page *page;

    spin_lock_irqsave(&region->lock, &irqflags);
    page = buddy_smallest(region, order);
    spin_unlock_irqrestore(&region->lock, &irqflags);

    return page;
}

static bool check_buddy(struct page *page, struct page *buddy, unsigned int order)
{
    if (buddy->type != PAGE_FREE)
        return false;

    if (order != buddy->order)
        return false;

    if (page_to_region(page) != page_to_region(buddy))
        return false;

    return true;
}

static void buddy_merge(struct region *region, struct page *page, unsigned int order)
{
    unsigned long buddy_pnr, pnr = page_to_nr(page);
    struct page_free *free;
    struct page *buddy;

    for (; order < PAGE_ORDER_MAX; ++order) {
        size_t merge_pnr;

        buddy_pnr = find_buddy(pnr, order);
        buddy = page + (buddy_pnr - pnr);

        if (!check_buddy(page, buddy, order))
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

static void buddy_free(struct region *region, struct page *page, unsigned int order)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&region->lock, &irqflags);
    buddy_merge(region, page, order);
    spin_unlock_irqrestore(&region->lock, &irqflags);
}

struct page *page_alloc(unsigned int order, gfp_t gfp)
{
    enum region_type region_type;
    struct region *region;
    struct page *page;

    if (unlikely(order > PAGE_ORDER_MAX))
        return NULL;

    region_type = gfp_to_region(gfp);
    region = &region_map[region_type];

    page = buddy_alloc(region, order, gfp);
    if (unlikely(!page))
        return NULL;

    if (gfp & GFP_ZERO)
        memset(page_to_va(page), 0, PAGE_SIZE << order);

    page->type = PAGE_USED;
    return page;
}
EXPORT_SYMBOL(page_alloc);

void page_free(struct page *page)
{
    enum region_type region_type;
    struct region *region;

    region_type = page_to_region(page);
    region = &region_map[region_type];
    buddy_free(region, page, page->order);
}
EXPORT_SYMBOL(page_free);

void page_add(struct region *region, struct page *page,
              unsigned int order, unsigned int nr)
{
    unsigned int size = 1 << order;
    for (; page && nr--; page += size)
    buddy_free(region, page, order);
}
EXPORT_SYMBOL(page_add);
