/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGE_H_
#define _MM_PAGE_H_

#include <mm/gfp.h>
#include <mm/mm_types.h> 
#include <mm/region.h>

#define order_nr(size, order)   (((size) >> PAGE_SHIFT) >> (order))

void *page_address(const struct page *page);
void page_add(struct region *region, struct page *page, uint order, uint order_nr);
struct page *page_alloc(uint order, gfp_t gfp);
void page_free(struct page *page, uint order);

#endif  /* _MM_PAGE_H_ */
