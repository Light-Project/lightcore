/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGE_H_
#define _MM_PAGE_H_

#include <bitops.h>
#include <mm/types.h>
#include <mm/gfp.h>
#include <mm/region.h>
#include <mm/memmodel.h>

static inline unsigned int size_to_order(size_t size)
{
    if (__builtin_constant_p(size)) {
        if (!size)
            return BITS_PER_LONG - PAGE_SHIFT;
        if (size < (1UL << PAGE_SHIFT))
            return 0;
        return ilog2((size) - 1) - PAGE_SHIFT + 1;
    }

    size -= 1;
    size >>= PAGE_SHIFT;

    return fls(size);
}

static inline unsigned int page_order(struct page *page)
{
    return page->order;
}

static inline size_t page_size(struct page *page)
{
    return PAGE_SIZE << page_order(page);
}

extern struct page *page_alloc(unsigned int order, gfp_t gfp);
extern void page_free(struct page *page);
extern void page_add(struct region *region, struct page *page, unsigned int order, unsigned int order_nr);
extern void *page_address(const struct page *page);

static inline void *get_free_pages(unsigned int order, gfp_t flags)
{
    struct page *page = page_alloc(order, flags & ~GFP_HIGHMEM);
    return page_to_va(page);
}

static inline void *get_free_page(gfp_t flags)
{
    return get_free_pages(0, flags);
}

static inline void *get_zeroed_page(gfp_t flags)
{
    return get_free_page(flags | GFP_ZERO);
}

static inline void free_page(void *addr)
{
    struct page *page = va_to_page(addr);
    page_free(page);
}

#endif /* _MM_PAGE_H_ */
