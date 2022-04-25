/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <mm/memblock.h>
#include <mm/region.h>
#include <mm/pgtable.h>
#include <mm/kmemleak.h>
#include <mm/vmem.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <mempool.h>
#include <ioremap.h>
#include <printk.h>

struct memory init_mm = {
    .pgdir = page_dir,
};

void __init mem_init(void)
{
    /* keep unavailable memory areas */
    memblock_reserve("ram-pad", CONFIG_RAM_BASE, CONFIG_RAM_PAD);
    memblock_reserve("kernel", NORMAL_OFFSET, IMAGE_SIZE);

    memblock_dump();
    memmodel_init();

    region_init();
    kmem_init();
    kcache_init();
    kmemleak_init();

    vmem_init();
    vmalloc_init();
    mempool_init();
    ioremap_init();
}
