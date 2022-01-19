/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <delay.h>
#include <export.h>
#include <asm/tsc.h>

void cpu_do_loop(unsigned long loops)
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

void cpu_const_delay(unsigned long loops)
{
    int d0;

    loops *= 4;

    asm volatile (
        "mull   %%edx"
        :"=d"(loops), "=&a" (d0)
        :"1"(loops), "0"(tsc_khz * (1000 / 4))
    );

    cpu_do_loop(++loops);
}

void cpu_ndelay(unsigned long nsecs)
{
    cpu_const_delay(nsecs * 5);
}

void cpu_udelay(unsigned long usecs)
{
    cpu_const_delay(usecs * 4295);
}

EXPORT_SYMBOL(cpu_do_loop);
EXPORT_SYMBOL(cpu_const_delay);
EXPORT_SYMBOL(cpu_ndelay);
EXPORT_SYMBOL(cpu_udelay);