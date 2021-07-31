/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMMODEL_H_
#define _MM_MEMMODEL_H_

#include <mm/mm_types.h>

#include <asm/page.h>

#define PAGE_ORDER_MAX  11

/* Flat Memory model */
#ifdef CONFIG_FLATMEM

struct mem_section {
};

#define PAGE_NR ((PAGE_MASK >> PAGE_SHIFT) + 1)
#define PAGE_BITMAP_NR (PAGE_NR / MSIZE)
extern struct page page_map[PAGE_NR];

#define page_to_nr(_page)   ((unsigned int)(((size_t)(_page) - (size_t)page_map) / sizeof(struct page)))
#define nr_to_page(pnr)     (page_map[pnr])

/* Sparce Memory model */
#elif CONFIG_SPARCEMEM

struct mem_section {
    struct list_head list;
};

#define page_to_nr(_page)   ((unsigned int)(((size_t)(_page) - (size_t)page_map) / sizeof(struct page)))
#define nr_to_page(pnr)

#define for_each_section()

#endif

#define va_to_pa(va)    ((phys_addr_t)(va) - CONFIG_PAGE_OFFSET)
#define pa_to_va(pa)    ((void *)((pa) + CONFIG_PAGE_OFFSET))

#define page_to_pa(page)    ((phys_addr_t)page_to_nr(page) << PAGE_SHIFT)
#define pa_to_page(pa)      (nr_to_page(pa >> PAGE_SHIFT))

#define page_to_va(page)    (pa_to_va(page_to_pa(page)))
#define va_to_page(address) (pa_to_page(va_to_pa(address)))

struct mem_section *mem_section_add(int region, phys_addr_t addr, size_t size);

#endif  /* _MM_MEMMODEL_H_ */
