/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <printk.h>

void __init mem_init(void)
{
    memblock_reserve(va_to_pa(&_ld_startup_start),
            &_ld_image_end - &_ld_startup_start);
    memmodel_init();
    region_init();
    kmem_init();
    vmem_init();
}
