/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <prandom.h>
#include <filter/ewma.h>

#define TEST_LOOP 100

static state ewma_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;
    uint16_t rawdata, filtered;

    EWMA_STATE(test, UINT16_MAX * 20 / 100);
    for (count = 0; count < TEST_LOOP; ++count) {
        rawdata = (uint16_t)prandom_value();
        filtered = ewma_update(&test, rawdata);
        kshell_printf(ctx, "ewma filter test%02d: %5d -> %5d\n",
                      count, rawdata, filtered);
    }

    return -ENOERR;
}

static struct selftest_command ewma_command = {
    .group = "filter",
    .name = "ewma",
    .desc = "ewma unit test",
    .testing = ewma_testing,
};

static state selftest_ewma_init(void)
{
    return selftest_register(&ewma_command);
}
kshell_initcall(selftest_ewma_init);
