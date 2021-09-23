/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/cache.h>

void cache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c7, 0\n"
        :::"ip"
    );
}

void dcache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c6, 0\n"
        :::"ip"
    );
}

void dcache_writeback_all(void)
{
    asm volatile (
    "1:  mrc    p15, 0, apsr_nzcv, c7, c14, 3\n"
        "bne    1b\n"
    );
}

void wcache_drain_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c10, 4\n"
        :::"ip"
    );
}
