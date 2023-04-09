/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <kernel.h>

static size_t patterns[] = {
    (size_t)0x0000000000000000ULL,
    (size_t)0xffffffffffffffffULL,
    (size_t)0x5555555555555555ULL,
    (size_t)0xaaaaaaaaaaaaaaaaULL,
    (size_t)0x1111111111111111ULL,
    (size_t)0x2222222222222222ULL,
    (size_t)0x4444444444444444ULL,
    (size_t)0x8888888888888888ULL,
    (size_t)0x3333333333333333ULL,
    (size_t)0x7777777777777777ULL,
    (size_t)0xccccccccccccccccULL,
    (size_t)0xeeeeeeeeeeeeeeeeULL,
};

static int pattern_test(size_t *addr, size_t size, size_t pattern)
{
    size_t *tmp, *end = addr + (size / MSIZE);

    for (tmp = addr; tmp < end; tmp++)
        *tmp = pattern;
    for (tmp = addr; tmp < end; tmp++)
        if (*tmp != pattern)
            return -1;
    return 0;
}

int memtest_fast(size_t *addr, size_t size)
{
    unsigned char i;

    pr_boot("memtest: size 0x%x @ 0x%x\n", size, addr);

    for (i = 0; i < 2; i++)
        if (pattern_test(addr, size, patterns[i]))
            return -1;

    return 0;
}

int memtest(size_t *addr, size_t size)
{
    unsigned char i;

    pr_boot("memtest: size 0x%x @ 0x%x\n", size, addr);

    for (i = 0; i < ARRAY_SIZE(patterns); i++)
        if (pattern_test(addr, size, patterns[i]))
            return -1;

    return 0;
}
