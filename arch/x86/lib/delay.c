/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <delay.h>
#include <export.h>
#include <asm/tsc.h>

void arch_proc_do_loop(unsigned long loops)
{
    asm volatile (
        "test   %0, %0  \n"
        "jz     3f      \n"
        "jmp    1f      \n"

        ".align 16		\n"
        "1: jmp 2f		\n"

        ".align 16		\n"
        "2:	dec %0		\n"
        "	jnz 2b		\n"
        "3:	dec %0		\n"
        ::"a"(loops)
    );
}

void arch_proc_const_delay(unsigned long loops)
{
    int d0;

    loops *= 4;

    asm volatile (
        "mull   %%edx"
        :"=d"(loops), "=&a" (d0)
        :"1"(loops), "0"(loops_per_tick * (1000 / 4))
    );

    arch_proc_do_loop(++loops);
}

void arch_proc_ndelay(unsigned long nsecs)
{
    arch_proc_const_delay(nsecs * 5);
}

void arch_proc_udelay(unsigned long usecs)
{
    arch_proc_const_delay(usecs * 4295);
}

EXPORT_SYMBOL(arch_proc_do_loop);
EXPORT_SYMBOL(arch_proc_const_delay);
EXPORT_SYMBOL(arch_proc_ndelay);
EXPORT_SYMBOL(arch_proc_udelay);
