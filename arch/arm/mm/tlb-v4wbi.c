/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <asm/tlb.h>

uint32_t tlb_rlock(void)
{
    uint32_t val;
    asm volatile (
        "mrc    p15, 0, %0, c10, c0, 1\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void tlb_wlock(uint32_t lock)
{
    asm volatile (
        "mcr    p15, 0, %0, c10, c0, 1\n\t"
        ::"r"(lock)
    );
}
