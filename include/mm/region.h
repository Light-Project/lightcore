/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_REGION_H_
#define _MM_REGION_H_

#include <list.h>
#include <mm/gfp.h>
#include <system/spinlock.h>

#define PAGE_ORDER_MAX  11

#ifndef __ASSEMBLY__

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
    struct list_head list;  /* free page list */
    size_t free_nr;         /* free page number */
};

struct region {
    const char *name;

    /* Free page list in this region */
    struct page_free page_free[PAGE_ORDER_MAX];
    spinlock_t lock;    /* page_free lock */
    
};

extern struct region region_map[];

#define for_each_region(region) \
        list_for_each_entry(region, &region_list, list)

static inline bool region_is_himem(enum region_type type)
{
#ifdef CONFIG_REGION_HIMEM
    return type == REGION_HIMEM;
#endif
    return false;
}

struct region *gfp_to_region(gfp_t gfp);

#endif	/* __ASSEMBLY__ */
#endif /* _MM_REGION_H_ */
