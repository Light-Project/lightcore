/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/highmem.h>
#include <mm/memmodel.h>
#include <mm/region.h>

void *page_address(const struct page *page)
{
    /*
     * If it's a kernel direct mapping,
     * we only need simple transformation.
     */
    if (page_to_region(page) != REGION_HIMEM)
        return page_to_va(page);

    return NULL;
}