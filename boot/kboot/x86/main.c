/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
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
#ifdef CONFIG_KBOOT_CON_VGA
    video_clear();
    pr_init(video_print);
#endif
#ifdef CONFIG_KBOOT_CON_SER
    serial_init();
    pr_init(serial_print);
#endif

    magic = readl(piggy_load);
    if (magic != PIGGY_MAGIC)
        panic("bad kernel image");

    /* Init Memory */
    heap_setup(heap_start, heap_size);
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
