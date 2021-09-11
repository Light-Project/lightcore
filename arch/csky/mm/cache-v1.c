/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <spinlock.h>
#include <asm/cache.h>

static __always_inline void
cache_line_op(size_t addr, uint32_t val)
{
    mtcr("cr22", addr);
    mtcr("cr17", val);
}

static __always_inline void
cache_all_op(uint32_t val)
{
    mtcr("cr17", val | CFR_CLR);
}

void icache_inval_all(void)
{
    cache_all_op(CFR_SELI | CFR_INV);
}

void dcache_writeback_all(void)
{
    cache_all_op(CFR_SELD | CFR_INV);
}
