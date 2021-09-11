/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/cache.h>
#include <asm/tlb.h>
#include <asm/proc.h>

void arch_mm_switch(uint32_t *addr)
{

    dcache_writeback_all();
    icache_inval_all();
    wcache_drain_all();

    tlb_inval_all();
}
