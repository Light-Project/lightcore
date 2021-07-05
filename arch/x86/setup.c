/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <types.h>

#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/page.h>

void pre_console_init();

#include <printk.h>

void arch_setup(const char *kernel_cmd)
{
    /* init pre_printk */
    pre_console_init();

    arch_gdt_setup();
    arch_idt_setup();

//     arch_page_setup();
}
