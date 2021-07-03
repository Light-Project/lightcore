/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  arch/arm/926ejs/proc.c
 *  arm926 interface
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-18      first version 
 * 
 */

#include <types.h>

void arch_irq_disable()
{
    asm volatile("cpsid i": : :"memory");
}

void arch_irq_enable()
{
    asm volatile("cpsie i": : :"memory");
}


void arch_halt()
{
    asm volatile("wfi":::"ip");
}


