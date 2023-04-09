/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <math.h>
#include <selftest.h>

static state prime_testing(struct kshell_context *ctx, void *pdata)
{
    ktime_t start = timekeeping_get_time();
    unsigned long value = 0, count = 0;

    do {
        value = prime_next(value);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    kshell_printf(ctx, "prime benchmark: %lu/s\n", count);

    return -ENOERR;
}

static struct selftest_command prime_command = {
    .group = "benchmark",
    .name = "prime",
    .desc = "benchmark prime",
    .testing = prime_testing,
};

static state selftest_prime_init(void)
{
    return selftest_register(&prime_command);
}
kshell_initcall(selftest_prime_init);
