/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>

void main(void)
{
    memset(bss_start, 0, bss_size);
    heap_init(heap_start, heap_size);
    
    kernel_map();
    
    extract_kernel(kernel_entry, piggy_start, piggy_size);
    kernel_start(kernel_entry);
}
