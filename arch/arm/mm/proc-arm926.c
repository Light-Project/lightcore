/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>

void arch_mm_switch(uint32_t *addr)
{
    asm volatile(
        "mov    ip, #0\n"

        /* invalidate D cache */
        "mcr    p15, 0, ip, c7, c6, 0\n"

        /* invalidate I cache */
        "mcr	p15, 0, ip, c7, c5, 0\n"

        /* Drain write buffer */
        "mcr	p15, 0, ip, c7, c10, 4\n"

        /* load page table pointer */
        "mcr    p15, 0, %0, c2, c0, 0\n"

        /* invalidate instruct and data tlb */
        "mcr    p15, 0, ip, c8, c7, 0\n"

        ::"r"(addr):"ip"
    );
}

void arch_halt()
{

}
