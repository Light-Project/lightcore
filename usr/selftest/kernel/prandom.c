/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP 100

static state prandom_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;
    uint32_t value;

    for (count = 0; count < TEST_LOOP; ++count) {
        value = prandom_value();
        kshell_printf(ctx, "prandom test%02u: %#08x\n", count, value);
    }

    return -ENOERR;
}

static struct selftest_command prandom_command = {
    .group = "kernel",
    .name = "prandom",
    .desc = "prandom unit test",
    .testing = prandom_testing,
};

static state selftest_prandom_init(void)
{
    return selftest_register(&prandom_command);
}
kshell_initcall(selftest_prandom_init);
