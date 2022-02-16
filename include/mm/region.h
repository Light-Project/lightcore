/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_REGION_H_
#define _MM_REGION_H_

#include <spinlock.h>
#include <mm/types.h>
#include <mm/gfp.h>

#define PAGE_ORDER_MAX  11

enum region_type {
#ifdef CONFIG_REGION_DMA
    REGION_DMA,
#endif
#ifdef CONFIG_REGION_DMA32
    REGION_DMA32,
#endif
    REGION_NORMAL,
#ifdef CONFIG_REGION_HIMEM
    REGION_HIMEM,
#endif
    REGION_NR_MAX,
};

struct page_free {
    struct list_head list;
    size_t free_nr;
};

struct region {
    const char *name;
    spinlock_t lock;
    struct page_free page_free[PAGE_ORDER_MAX + 1];
};

extern struct region region_map[REGION_NR_MAX];
extern enum region_type gfp_to_region(gfp_t gfp);
extern enum region_type pa_to_region(phys_addr_t pa);
extern enum region_type page_to_region(const struct page *page);
extern void __init region_init(void);

#endif /* _MM_REGION_H_ */
