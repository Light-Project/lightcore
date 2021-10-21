/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMMODEL_H_
#define _MM_MEMMODEL_H_

#include <mm/mm_types.h>
#include <asm/page.h>

/* Flat Memory model */
#ifdef CONFIG_FLATMEM

#define PAGE_NR ((PAGE_MASK >> PAGE_SHIFT) + 1)
#define PAGE_BITMAP_NR (PAGE_NR / MSIZE)
extern struct page page_map[];

#define page_to_nr(pg)  ((unsigned long)((pg) - page_map))
#define nr_to_page(pnr) (&page_map[pnr])

static inline void memmodel_init(void) {}

/* Sparce Memory model */
#elif CONFIG_SPARCEMEM

#define SECTIONS_WIDE (PHYS_SHIFT - SPARCE_SHIFT)
#define SECTIONS_PFN_SHIFT (SPARCE_SHIFT - PAGE_SHIFT)

#define SECTIONS_NR (1UL << SECTIONS_WIDE)
#define PAGES_PER_SECTION (1UL << SECTIONS_PFN_SHIFT)

struct sparce_block {
    struct page *page_map;
    size_t present:1;
};

extern struct sparce_block sparce_map[];

#define sparce_base(sp) \
    ((unsigned long)((sp) - sparce_map) << SECTIONS_PFN_SHIFT)

static inline unsigned long
sparce_page_nr(const struct sparce_block *sparce, const struct page *page)
{
    return sparce_base(sparce) + (page - sparce->page_map);
}

static inline struct page *
sparce_nr_page(const struct sparce_block *sparce, unsigned long pnr)
{
    if(!sparce->present)
        return NULL;
    return &sparce->page_map[pnr - sparce_base(sparce)];
}

#define page_to_nr(pg) ({                                   \
    const struct page *__page = (pg);                       \
    sparce_page_nr(&sparce_map[__page->sparce_nr], __page); \
})

#define nr_to_page(pnr) ({                                                      \
    unsigned int __page_nr = pnr;                                               \
    sparce_nr_page(&sparce_map[(__page_nr) >> SECTIONS_PFN_SHIFT], __page_nr);  \
})

#endif  /* CONFIG_SPARCEMEM */

void memmodel_init(void);

#endif  /* _MM_MEMMODEL_H_ */
