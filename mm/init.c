/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>

void __init mem_init(void)
{
    mem_reserve(va_to_pa(&_ld_text_start),
        (size_t)&_ld_image_end - (size_t)&_ld_text_start);
    mem_reserve(0, PAGE_SIZE);
    
    region_init();
    kmem_init();

    vmem_init();
}
