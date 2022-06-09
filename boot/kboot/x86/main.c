/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <memory.h>
#include <kboot.h>
#include <asm/io.h>
#include <arch/x86/seg.h>

#ifdef CONFIG_ARCH_X86_32
#define KERNEL_SEGMENT GDT_ENTRY_KERNEL_CS_BASE
#else
#define KERNEL_SEGMENT GDT_LENTRY_KERNEL_CS_BASE
#endif

struct bootparam bootparam __bootparam;

asmlinkage void main(void)
{
    struct boot_head *head;
    uint32_t magic;

    /* Init console */
    console_clear();
    pr_init(console_print);

    magic = readl(piggy_load);
    if (magic != PIGGY_MAGIC)
        panic("bad kernel image");

    /* Init Memory */
    heap_init(heap_start, heap_size);
    segment_init();
    kernel_map();

    /* Extract kernel */
    extract_kernel(pa_to_va(NORMAL_OFFSET), piggy_load + 4, piggy_size);
    head = pa_to_va(NORMAL_OFFSET);
    head->cmd = cmd_line_ptr ? (uintptr_t)pa_to_va(cmd_line_ptr) : 0;
    head->params = (uintptr_t)pa_to_va(&bootparam);

    /* Boot kernel */
    pr_boot("boot to kernel...\n");
    kernel_start(KERNEL_SEGMENT, pa_to_va(NORMAL_OFFSET));
}
