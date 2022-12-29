/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/page.h>
#include <asm/tlb.h>
#include <asm/pcid.h>

void tlb_inval_one(size_t addr)
{
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void tlb_inval_range(size_t addr, size_t size)
{
    for (; size; addr += PAGE_SIZE, size -= PAGE_SIZE)
        tlb_inval_one(addr);
}
