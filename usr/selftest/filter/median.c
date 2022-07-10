/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <prandom.h>
#include <filter/median.h>

#define TEST_LOOP 100

static state median_testing(struct kshell_context *ctx, void *pdata)
{
    struct median_state *test = pdata;
    unsigned int count;
    short rawdata, filtered;

    for (count = 0; count < TEST_LOOP; ++count) {
        rawdata = (short)prandom_value();
        filtered = median_update(test, rawdata);
        kshell_printf(ctx, "median filter test%02d: %6d -> %6d\n",
                      count, rawdata, filtered);
    }

    return -ENOERR;
}

static void *median_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return median_alloc(TEST_LOOP / 10, GFP_KERNEL);
}

static void median_release(struct kshell_context *ctx, void *pdata)
{
    median_free(pdata);
}

static struct selftest_command median_command = {
    .group = "filter",
    .name = "median",
    .desc = "median unit test",
    .testing = median_testing,
    .prepare = median_prepare,
    .release = median_release,
};

static state selftest_median_init(void)
{
    return selftest_register(&median_command);
}
kshell_initcall(selftest_median_init);
