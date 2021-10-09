/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <mm/region.h>
#include <mm/highmem.h>

void *page_address(const struct page *page)
{
    /*
     * If it's a kernel direct mapping, we only need a
     * simple transformation
     */
    // if(page->region_type != REGION_HIMEM)
        return page_to_va(page);

    return NULL;
}