/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <size.h>
#include <kboot.h>

void main(void)
{
    /* clean bss */
    memset(bss_start, 0, bss_size);

    console_clear();

    /* Init console */
    pr_init(console_print);

    /* Setup segment */
    segment_init();

    /* Initialization heap */
    heap_init(heap_start, heap_size);

    /* Initialization MMU */
    kernel_map();

    extract_kernel(pa_to_va(NORMAL_OFFSET), piggy_start, piggy_size);
    pr_boot("Boot to kernel...");

    /* jmp to kernel */
    kernel_start(pa_to_va(NORMAL_OFFSET));
}
