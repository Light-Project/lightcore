/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "mmodule"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <printk.h>
#include <panic.h>
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
    phys_addr_t addr;
    size_t size;

    addr = align_low(reg->range.start, SPARCE_SIZE);
    size = align_high(reg->range.size, SPARCE_SIZE);

    for (sparce = base_sparce(addr); size; size -= SPARCE_SIZE, ++sparce) {
        if (sparce->present)
            continue;
        sparce->present = true;
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

    if (unlikely(!page))
        panic("sparce alloc failed");

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
    pr_info("sparce section number: %lu\n", sparce_nr);

    sparce_alloc();
}

#endif  /* CONFIG_SPARCEMEM */
