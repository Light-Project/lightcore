/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>

void main(size_t mem_start, size_t dtb_start)
{
    struct boot_head *head;

    memset(bss_start, 0, bss_size);
    heap_setup(heap_start, heap_size);

    /* Init Memory */
    kernel_map();

    /* extract kernel */
    extract_kernel(pa_to_va(NORMAL_OFFSET), piggy_start, piggy_size);
    head = kernel_entry;
    head->dtb = dtb_start ? CONFIG_PAGE_OFFSET + dtb_start : 0;

    pr_boot("boot to kernel...\n");
    kernel_start(pa_to_va(NORMAL_OFFSET));
}
