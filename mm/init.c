/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <mm/memblock.h>
#include <mm/region.h>
#include <mm/vmem.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <ioremap.h>
#include <printk.h>

struct memory init_mm = {
};

void __init mem_init(void)
{
    /* keep unavailable memory areas */
    memblock_reserve("ram-pad", 0, CONFIG_RAM_BASE + CONFIG_RAM_PAD);
    memblock_reserve("kernel", va_to_pa(&_ld_startup_start),
                       &_ld_image_end - &_ld_startup_start);

    memmodel_init();
    region_init();

    kmem_init();
    kcache_init();

    vmem_init();
    vmalloc_init();
    ioremap_init();
}
