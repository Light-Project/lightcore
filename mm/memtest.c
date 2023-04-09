/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "memtest"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <memtest.h>
#include <kernel.h>
#include <ioops.h>
#include <export.h>

static size_t memtest_patterns[10] = {
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

#if BITS_PER_LONG == 32
# define memtest_read(addr) readl(addr)
# define memtest_write(addr, value) writel(addr, value)
#else /* BITS_PER_LONG == 64 */
# define memtest_read(addr) readq(addr)
# define memtest_write(addr, value) writeq(addr, value)
#endif /* BITS_PER_LONG == 64 */

void memtest_once(memtest_report_t func, size_t pattern,
                  void *block, size_t size, size_t step)
{
    const size_t incr = sizeof(pattern);
    size_t bstart, bsize = 0;
    size_t index;

    step = max(align_high(step, incr), incr);
    align_ptr_high_adj(block, step);

    for (index = 0; index < size; index += incr)
        memtest_write(block + index, pattern);

    for (index = 0; index < size;) {
        if (likely(memtest_read(block + index) == pattern)) {
            index += incr;
            continue;
        }

        align_low_adj(index, step);
        if (bsize && index == bstart + bsize)
            bsize += step;
        else {
            if (bsize)
                func(block + bstart, bsize);
            bstart = index;
            bsize = step;
        }
        index += step;
    }

    if (bsize)
        func(block + bstart, bsize);
}
EXPORT_SYMBOL(memtest_once);

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
