/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  arch/arm/mm/mm.c
 *  arm mm
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-04      first version 
 * 
 */

#include <arch.h>

void arch_mmu_enable()
{
    asm volatile(
        "mrc    p15, 0, ip, c1, c0, 0 \n\t"
        "orr    ip, ip, #0x1 \n\t"
        "mcr    p15, 0, ip, c1, c0, 0 \n\t"
        :::"ip"
    );
}

void arch_mmu_disable()
{
    asm volatile
    (
        "mrc    p15, 0, ip, c1, c0, 0 \n\t"
        "bic    ip, ip, #0x1 \n\t"
        "mcr    p15, 0, ip, c1, c0, 0 \n\t"
        :::"ip"
    );
}

uint32_t arch_mmu_ttb_read()
{
    uint32_t val;
    asm volatile
    (
        "mrc    p15, 0, %0, c2, c0, 0\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void arch_mmu_ttb_write(uint32_t *addr)
{
    asm volatile
    (
        "mcr   p15, 0, %0, c2, c0, 0\n\t"
        ::"r"(addr):"memory"
    );
}

void arch_mmu_link_section(uint32_t *ttb_table, uint32_t va, uint32_t pa, uint32_t size, uint8_t type)
{
    va   >>= 20;
    pa   >>= 20;
    size >>= 20;
    type &= 0x03;
    for(size; size > 0; size--)
    {
        ttb_table[va] = (pa<<20)|(0x3<<10)|(0x0<<5)|(type<<2)|(0x2<<0);
        va++;
        pa++;
    }
}
void arch_mmu_link_page(uint32_t *ttb_table, uint32_t va, uint32_t pa, uint32_t size, uint8_t type)
{
    
    
}
