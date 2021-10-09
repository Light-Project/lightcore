/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <kmalloc.h>
#include <mm/memblock.h>
#include <mm/region.h>
#include <mm/vmem.h>
#include <printk.h>

void __init mem_init(void)
{
    /* keep unavailable memory areas */
    memblock_reserve(0, CONFIG_RAM_BASE + CONFIG_RAM_PAD);
    memblock_reserve(va_to_pa(&_ld_startup_start),
             &_ld_image_end - &_ld_startup_start);

    /* initialization buddy */
    memmodel_init();
    region_init();
    kmem_init();

    kcache_init();
    vmem_init();
}
