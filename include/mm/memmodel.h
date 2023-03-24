/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_MEMMODEL_H_
#define _MM_MEMMODEL_H_

#include <mm/types.h>
#include <asm/page.h>

#ifdef CONFIG_FLATMEM
/* Flat Memory model */

#define PAGE_NR         ((PAGE_MASK >> PAGE_SHIFT) + 1)
#define PAGE_BITMAP_NR  (PAGE_NR / MSIZE)

#define page_to_nr(pg)  ((unsigned long)((pg) - page_map))
#define nr_to_page(pnr) (&page_map[pnr])

extern struct page page_map[];
static inline void memmodel_init(void) {}

#elif CONFIG_SPARCEMEM
/* Sparce Memory model */

#define SPARCE_SHIFT        CONFIG_SPARCE_SHIFT
#define SPARCE_SIZE         (1UL << SPARCE_SHIFT)
#define SPARCE_MASK         (SPARCE_SIZE - 1)

#define SECTIONS_WIDE       (PHYS_SHIFT - SPARCE_SHIFT)
#define SECTIONS_PFN_SHIFT  (SPARCE_SHIFT - PAGE_SHIFT)

#define SECTIONS_NR         (1UL << SECTIONS_WIDE)
#define PAGES_PER_SECTION   (1UL << SECTIONS_PFN_SHIFT)

struct sparce_block {
    struct page *page_map;
    size_t present:1;
};

extern struct sparce_block sparce_map[];

#define base_sparce(base) \
    (sparce_map + ((base) >> SPARCE_SHIFT))

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
    if (unlikely(!sparce->present))
        return NULL;
    return &sparce->page_map[pnr - sparce_base(sparce)];
}

#define page_to_nr(pg) ({                                                       \
    const struct page *__page = (pg);                                           \
    sparce_page_nr(&sparce_map[__page->sparce_nr], __page);                     \
})

#define nr_to_page(pnr) ({                                                      \
    unsigned int __page_nr = pnr;                                               \
    sparce_nr_page(&sparce_map[(__page_nr) >> SECTIONS_PFN_SHIFT], __page_nr);  \
})

extern void __init memmodel_init(void);

#endif /* CONFIG_SPARCEMEM */

#define page_to_pa(page)    (page_to_nr(page) << PAGE_SHIFT)
#define page_to_va(page)    (pa_to_va(page_to_pa(page)))
#define pa_to_page(paddr)   (nr_to_page(paddr >> PAGE_SHIFT))
#define va_to_page(vaddr)   (pa_to_page(va_to_pa(vaddr)))

#endif /* _MM_MEMMODEL_H_ */
