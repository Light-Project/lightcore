/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGE_H_
#define _MM_PAGE_H_

#include <mm/types.h>
#include <mm/gfp.h>
#include <mm/region.h>
#include <asm/bitops.h>
#include <asm/page.h>

static inline unsigned int to_order(size_t size)
{
    if (__builtin_constant_p(size)) {
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

extern void *page_address(const struct page *page);

extern struct page *page_alloc(uint order, gfp_t gfp);
extern void page_free(struct page *page);
extern void page_add(struct region *region, struct page *page, uint order, uint order_nr);

#endif  /* _MM_PAGE_H_ */
