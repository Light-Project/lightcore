/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/memblock.h>
#include <asm/doublefault.h>
#include <asm/segment.h>
#include <asm/idt.h>
#include <asm/e820.h>
#include <asm/tsc.h>
#include <asm/pgtable.h>
#include <asm/header.h>
#include <asm-generic/header.h>

void cpu_setup(void)
{

}

void arch_setup(void)
{
    struct bootparam *parm = (void *)(size_t)boot_head.params;

    idt_setup();
    gdt_setup();

    arch_page_setup();
    doublefault_init();

    tsc_init();

    e820_init(&parm->e820_table);
    memblock_reserve("bios", 0xa0000, 0x60000);
    memblock_reserve("isa-hole", 0xf00000, 0x100000);
}
