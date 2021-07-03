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
    asm volatile(
        "mrs    ip, cpsr\n\t"
        "orr    ip, #0xc0\n\t"
        "msr    cpsr, ip"
        :::"ip"
    );
}

void arch_irq_enable()
{
    asm volatile(
        "mrs    ip, cpsr\n\t"
        "bic    ip, #0xc0\n\t"
        "msr    cpsr, ip"
        :::"ip"
    );
}

void arch_mm_switch(uint32_t *addr)
{
    asm volatile(
        "mov    ip, #0\n\r"

        /* invalidate D cache */
        "mcr    p15, 0, ip, c7, c6, 0\n\r"

        /* invalidate I cache */
        "mcr	p15, 0, ip, c7, c5, 0\n\r"

        /* Drain write buffer */
        "mcr	p15, 0, ip, c7, c10, 4\n\r"

        /* load page table pointer */
        "mcr    p15, 0, %0, c2, c0, 0\n\r"

        /* invalidate instruct and data tlb */
        "mcr    p15, 0, ip, c8, c7, 0\n\r"

        ::"r"(addr):"ip"
    );
}

void arch_halt()
{
    
}
