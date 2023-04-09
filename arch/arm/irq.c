/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>

#include <irqflags.h>
#include <asm/entry.h>
#include <asm/regs.h>
#include <asm/pgtable.h>

extern char vector_table[];

void arch_irq_set(void (*fn)(struct regs *))
{
}

void __init arch_irq_init(void)
{

}
