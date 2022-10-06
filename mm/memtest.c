/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "memtest"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <memtest.h>
#include <kernel.h>
#include <printk.h>
#include <export.h>

static size_t memtest_patterns[] = {
    (size_t)0x0000000000000000ULL, /* 0000 */
    (size_t)0xffffffffffffffffULL, /* 1111 */
    (size_t)0x1111111111111111ULL, /* 0001 */
    (size_t)0x2222222222222222ULL, /* 0010 */
    (size_t)0x4444444444444444ULL, /* 0100 */
    (size_t)0x8888888888888888ULL, /* 1000 */
    (size_t)0x5555555555555555ULL, /* 0101 */
    (size_t)0xaaaaaaaaaaaaaaaaULL, /* 1010 */
    (size_t)0x3333333333333333ULL, /* 0011 */
    (size_t)0xccccccccccccccccULL, /* 1100 */
};

static void memtest_once(memtest_report_t func, size_t pattern,
                         size_t *block, size_t size, size_t step)
{
    const size_t incr = sizeof(pattern);
    size_t bbstart = 0, bblast = 0;
    size_t index;

    block = align_ptr_high(block, incr);
    size /= incr, step = step / incr - 1;

    for (index = 0; index < size; ++index)
        block[index] = pattern;

    for (index = 0; index < size; ++index) {
        if (likely(block[index] == pattern))
            continue;

        if (index == bblast + incr) {
            bblast += incr;
            continue;
        }

        if (bbstart)
            func(block + bbstart, (bblast - bbstart + 1) * incr);

        bbstart = bblast = index;
        index += step;
    }

    if (bbstart)
        func(block + bbstart, (bblast - bbstart + 1) * incr);
}

void memtest(memtest_report_t report, unsigned int level,
             void *block, size_t size, size_t step)
{
    unsigned int index;
    size_t pattern;

    for (index = 0; index < level; ++index) {
        pattern = memtest_patterns[index % ARRAY_SIZE(memtest_patterns)];
        memtest_once(report, pattern, block, size, step);
    }
}
EXPORT_SYMBOL(memtest);
