/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <kboot.h>

void __noreturn main(void)
{
    struct boot_head *head;

    /* clean bss */
    memset(bss_start, 0, bss_size);

    /* Init console */
    console_clear();
    pr_init(console_print);

    /* Init heap */
    heap_init(heap_start, heap_size);

    /* Init Memory */
    segment_init();
    kernel_map();

    /* extract kernel */
    extract_kernel(pa_to_va(NORMAL_OFFSET), piggy_start, piggy_size);
    head = pa_to_va(NORMAL_OFFSET);
    head->params = (size_t)pa_to_va(&bootparam);

    pr_boot("boot to kernel...\n");
    kernel_start();
}
