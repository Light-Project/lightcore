/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGE_H_
#define _MM_PAGE_H_

#include <mm/gfp.h>
#include <mm/region.h>
#include <mm/vmem.h>

#include <mm/page_type.h>   /*  */
#include <mm/memmodel.h>    /* Memory model definition */

#include <asm/page.h>

#define va_to_pa(va)    ((phys_addr_t)va - CONFIG_PAGE_OFFSET)
#define pa_to_va(pa)    ((void *)(pa + CONFIG_PAGE_OFFSET))

#define PAGE_NR             ((PAGE_MASK >> PAGE_SHIFT) + 1)

#ifndef __ASSEMBLY__

extern struct page page_map[];

struct page *buddy_alloc(int order, gfp_t gfp);
void buddy_free(struct page *page);
void buddy_add(struct region *region, int nr, int order, struct page *page);

void *page_to_address(const struct page *page);
struct page *page_alloc(unsigned int order, gfp_t gfp);
void page_free(struct page *page, unsigned int order);

void page_init(void);

#endif	/* __ASSEMBLY__ */
#endif
