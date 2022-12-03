/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/memblock.h>
#include <asm/stackprotector.h>
#include <asm/doublefault.h>
#include <asm/segment.h>
#include <asm/idt.h>
#include <asm/e820.h>
#include <asm/tsc.h>
#include <asm/pgtable.h>
#include <asm/header.h>
#include <export.h>

#ifdef CONFIG_STACKPROTECTOR
DEFINE_PERCPU(unsigned long, __stack_chk_guard);
EXPORT_SYMBOL(__stack_chk_guard);
#endif

void arch_setup(void)
{
    struct bootparam *parm = (void *)(size_t)boot_head.params;

    arch_idt_setup();
    arch_gdt_setup();

#ifdef CONFIG_ARCH_X86_32
    doublefault_init();
#endif

#ifdef CONFIG_ARCH_X86_64
    arch_setup_nx();
    arch_report_nx();
#endif

    arch_tsc_setup();

    arch_e820_setup(&parm->e820_table);
    memblock_reserve("bios", 0xa0000, 0x60000);
    memblock_reserve("isa-hole", 0xf00000, 0x100000);

    arch_page_setup();
}
