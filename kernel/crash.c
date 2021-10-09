/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <printk.h>

#include <asm/proc.h>
#include <asm/irq.h>

void __noreturn panic(const char* fmt, ...)
{
    char buf[256];
    va_list args;

    cpu_irq_disable();

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printk("Kernel panic: %s :(\n", buf);
    printk("---[ end Kernel panic ]---\n");

    cpu_halt();
}
