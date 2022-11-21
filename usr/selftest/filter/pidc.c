/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <prandom.h>
#include <filter/pidc.h>

#define TEST_LOOP 100

static state pidc_testing(struct kshell_context *ctx, void *pdata)
{
    PIDC_STATE(pctx, INT16_MAX * 0.6, INT16_MAX * 0.5, INT16_MAX * 0.125, 0, 1000);
    int32_t retval, curr;
    unsigned int count;

    for (count = 0, curr = 100; count < TEST_LOOP; ++count) {
        retval = pidc_update(&pctx, 1000, curr);
        curr += retval - 200;
        kshell_printf(ctx, "pidc static test%02d: %d -> %d\n", count, retval, curr);
    }

    pidc_reset(&pctx);
    for (count = 0, curr = 100; count < TEST_LOOP; ++count) {
        retval = pidc_update(&pctx, 1000, curr);
        curr += retval - prandom_value() % 200;
        kshell_printf(ctx, "pidc dynamic test%02d: %d -> %d\n", count, retval, curr);
    }

    return -ENOERR;
}

static struct selftest_command pidc_command = {
    .group = "filter",
    .name = "pidc",
    .desc = "pid controller unit test",
    .testing = pidc_testing,
};

static state selftest_pidc_init(void)
{
    return selftest_register(&pidc_command);
}
kshell_initcall(selftest_pidc_init);
