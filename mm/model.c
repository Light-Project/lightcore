/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm/memmodel.h>
#include <mm/memblock.h>

#if defined(CONFIG_FLATMEM)

struct page page_map[PAGE_NR];

#elif defined(CONFIG_SPARCEMEM)

struct sparce_block sparce_map[SECTIONS_NR];
static size_t sparce_nr;

static bool sparce_present(struct memblock_region *reg)
{
    struct sparce_block *sparce;
    unsigned int count;

    sparce = &sparce_map[reg->addr >> SPARCE_SHIFT];
    count = (reg->size >> SPARCE_SHIFT) + 1;

    while (count--) {
        if (sparce->present)
            continue;
        sparce++->present = true;
        sparce_nr++;
    }

    return false;
}

static void sparce_page_setup(struct page *page, unsigned int snr)
{
    unsigned int count;
    for (count = 0; count < PAGES_PER_SECTION; ++count)
        page++->sparce_nr = snr;
}

static void sparce_alloc(void)
{
    struct page *page;
    unsigned int count;
    size_t size;

    size = sparce_nr * PAGES_PER_SECTION * sizeof(*page);
    page = memblock_alloc(size);

    for (count = 0; count < SECTIONS_NR; ++count) {
        if (!sparce_map[count].present)
            continue;
        sparce_page_setup(page, count);
        sparce_map[count].page_map = page;
        page += PAGES_PER_SECTION;
    }
}

void memmodel_init(void)
{
    memblock_takeover(MEMBLOCK_USABLE, sparce_present);
    sparce_alloc();
}

#endif  /* CONFIG_SPARCEMEM */
