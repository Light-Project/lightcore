/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <random.h>
#include <selftest.h>

#define TEST_LOOP 100

static state random_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;
    long value;

    for (count = 0; count < TEST_LOOP; ++count) {
        value = random_long();
        kshell_printf(ctx, "random test%02u: %lx\n", count, value);
    }

    return -ENOERR;
}

static struct selftest_command random_command = {
    .group = "kernel",
    .name = "random",
    .desc = "random unit test",
    .testing = random_testing,
};

static state selftest_random_init(void)
{
    return selftest_register(&random_command);
}
kshell_initcall(selftest_random_init);
