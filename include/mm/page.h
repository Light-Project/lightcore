/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGE_H_
#define _MM_PAGE_H_

#include <mm/gfp.h>
#include <mm/mm_types.h>
#include <mm/region.h>
#include <asm/bitops.h>

static inline int to_order(size_t size)
{
    if (__builtin_constant_p(size)) {

    }

    size -= 1;
	size >>= PAGE_SHIFT;

#if BITS_PER_LONG == 32
    return fls(size);
#elif BITS_PER_LONG == 64
    return fls64(size);
#endif
}


void *page_address(const struct page *page);
void page_add(struct region *region, struct page *page, uint order, uint order_nr);
struct page *page_alloc(uint order, gfp_t gfp);
void page_free(struct page *page);

#endif  /* _MM_PAGE_H_ */
