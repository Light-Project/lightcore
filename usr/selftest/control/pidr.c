/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <prandom.h>
#include <control/pidr.h>

#define TEST_LOOP 100

static state pidr_testing(struct kshell_context *ctx, void *pdata)
{
    PIDR_STATE(pctx, 600, 1000, 500, 1000, 125, 1000, 0, 1000);
    int32_t curr, retval;
    unsigned int count;

    for (count = 0, curr = 100; count < TEST_LOOP; ++count) {
        retval = pidr_update(&pctx, 1000, curr);
        curr += retval - 200;
        kshell_printf(ctx, "pidr static test%02d: %d -> %d\n", count, retval, curr);
    }

    pidr_reset(&pctx);
    for (count = 0, curr = 100; count < TEST_LOOP; ++count) {
        retval = pidr_update(&pctx, 1000, curr);
        curr += retval - prandom_value() % 200;
        kshell_printf(ctx, "pidr dynamic test%02d: %d -> %d\n", count, retval, curr);
    }

    return -ENOERR;
}

static struct selftest_command pidr_command = {
    .group = "filter",
    .name = "pidr",
    .desc = "pid controller unit test",
    .testing = pidr_testing,
};

static state selftest_pidr_init(void)
{
    return selftest_register(&pidr_command);
}
kshell_initcall(selftest_pidr_init);
