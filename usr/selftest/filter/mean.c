/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <prandom.h>
#include <filter/mean.h>

#define TEST_LOOP 100

static state mean_testing(struct kshell_context *ctx, void *pdata)
{
    struct mean_state *test = pdata;
    unsigned int count;
    short rawdata, filtered;

    for (count = 0; count < TEST_LOOP; ++count) {
        rawdata = (short)prandom_value();
        filtered = mean_update(test, rawdata);
        kshell_printf(ctx, "mean filter test%02d: %6d -> %6d\n",
                      count, rawdata, filtered);
    }

    return -ENOERR;
}

static void *mean_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return mean_alloc(TEST_LOOP / 10, GFP_KERNEL);
}

static void mean_release(struct kshell_context *ctx, void *pdata)
{
    mean_free(pdata);
}

static struct selftest_command mean_command = {
    .group = "filter",
    .name = "mean",
    .desc = "mean unit test",
    .testing = mean_testing,
    .prepare = mean_prepare,
    .release = mean_release,
};

static state selftest_mean_init(void)
{
    return selftest_register(&mean_command);
}
kshell_initcall(selftest_mean_init);
