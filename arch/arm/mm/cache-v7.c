/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <asm/cache.h>

void dcache_inval_all(void)
{
    dcache_inval_range(0, ~0);
}

void dcache_writeback_all(void)
{
    dcache_writeback_range(0, ~0);
}
