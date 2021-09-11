/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <power.h>
#include <printk.h>
#include <asm/irq.h>

void __kassert(const char *expr, const char *file,
               int line, const char *func)
{
    panic("kassert: %s:%d:%s %s", file, line, func, expr);
}

void panic(const char* fmt, ...)
{
    char buf[256];
    va_list args;

    arch_irq_disable();

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printk("Kernel panic: %s :(\n", buf);
    printk("---[ end Kernel panic ]---\n");

    power_halt();
}
