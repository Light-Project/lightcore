/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/memblock.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/pgtable.h>
#include <asm/tsc.h>

void arch_setup(void)
{
    idt_setup();
    gdt_setup();
    arch_page_setup();

    tsc_init();

    memblock_reserve(0xa0000, 0x60000);
    memblock_reserve(0xf00000, 0x100000);
}
