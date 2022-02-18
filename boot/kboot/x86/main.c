/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <memory.h>
#include <kboot.h>
#include <arch/x86/seg.h>

asmlinkage void main(void)
{
    struct boot_head *head;

    /* Init console */
    console_clear();
    pr_init(console_print);

    /* Init Memory */
    heap_init(heap_start, heap_size);
    segment_init();
    kernel_map();

    /* Extract kernel */
    extract_kernel(pa_to_va(NORMAL_OFFSET), (void *)code32_start, piggy_size);
    head = pa_to_va(NORMAL_OFFSET);
    head->params = (size_t)pa_to_va(&bootparam);

    pr_boot("boot to kernel...\n");
    kernel_start(GDT_ENTRY_KERNEL_CS_BASE, (size_t)pa_to_va(NORMAL_OFFSET));
}
