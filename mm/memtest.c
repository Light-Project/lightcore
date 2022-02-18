/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "memtest: " fmt

#include <memory.h>
#include <mm/memblock.h>
#include <printk.h>

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

static void bad_block(void *start, size_t size)
{
    pr_warn("Bad block size %ld @ %p\n", size, start);
    memblock_reserve("bad-block", va_to_pa(start), size);
}

static void memtest_once(size_t pattern, size_t *start, size_t *end)
{
    size_t *walk, *bad_start = NULL;

    for (walk = start; walk < end; ++walk)
        *walk++ = pattern;

    for (walk = start; walk < end; ++walk) {
        if (likely(*walk == pattern))
            continue;
        /* Bad memory start */
        if (!bad_start)
            bad_start = walk;
        /* Bad memory end */
        if (*(walk + 1) == pattern) {
            bad_block(walk, (size_t)walk - (size_t)bad_start + 1);
            bad_start = NULL;
        }
    }
}

void __init memtest(void *block, size_t size)
{
    unsigned int c;
    for (c = 0; c < ARRAY_SIZE(patterns); ++c)
    memtest_once(patterns[c], block, block + size);
}
