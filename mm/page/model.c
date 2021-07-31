/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/page.h>
#include <mm/memmodel.h>
#include <mm/region.h>

#ifdef CONFIG_FLATMEM

size_t page_bitmap[PAGE_BITMAP_NR];
struct page page_map[PAGE_NR];
struct mem_section section[REGION_NR_MAX];

#elif CONFIG_SPARCEMEM

#endif  /* CONFIG_SPARCEMEM */
