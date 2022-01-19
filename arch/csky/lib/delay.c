/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <types.h>
#include <export.h>
#include <asm/delay.h>

void cpu_do_loop(unsigned long loops)
{
    asm volatile (
		"mov    r0, r0  \n"
		"1:     declt %0\n"
		"bf	    1b"
		: "=r"(loops)
		: "0"(loops)
    );
}

void cpu_const_delay(unsigned long loops)
{
    cpu_do_loop(((uint64_t)loops * loops_per_tick * CONFIG_SYSTICK_FREQ) >> 32);
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
