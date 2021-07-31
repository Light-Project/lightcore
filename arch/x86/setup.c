/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/pgtable.h>

#include <printk.h>

void pre_console_init(void);

void arch_setup(void)
{
    /* init pre_printk */
    pre_console_init();

    arch_gdt_setup();
    arch_idt_setup();

    arch_page_setup();
}
