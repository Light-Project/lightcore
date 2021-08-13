/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <kboot.h>

static size_t patterns[] = {
    (size_t)0x0000000000000000ULL,
    (size_t)0xffffffffffffffffULL,
    (size_t)0x5555555555555555ULL,
    (size_t)0xaaaaaaaaaaaaaaaaULL,
};

int memtest(size_t *addr, size_t size)
{
    pr_boot("memtest: size 0x%x @ 0x%x\n", addr, size);

    size /= MSIZE;
    for (; size--; ++addr)
    for (unsigned char i = 0; i < ARRAY_SIZE(patterns); i++) {
        *addr = patterns[i];
        if (*addr != patterns[i])
           return -1;
    }
    return 0;
}
