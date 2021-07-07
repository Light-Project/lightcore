/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Memory region management
 */

#include <mm/region.h>

struct region region_map[REGION_NR_MAX];

struct region *gfp_to_region(gfp_t gfp)
{
    switch(gfp & GFP_REGION_MASK) {
#ifdef CONFIG_REGION_DMA
        case GFP_DMA:
            return &region_map[REGION_DMA];
#endif
#ifdef CONFIG_REGION_DMA32
        case GFP_DMA32:
            return &region_map[REGION_DMA32];
#endif
#ifdef CONFIG_REGION_HIMEM
        case GFP_HIGHMEM:
            return &region_map[REGION_HIMEM];
#endif
    }
    return &region_map[REGION_NORMAL];
}
