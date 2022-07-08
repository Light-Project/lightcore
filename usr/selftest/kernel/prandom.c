/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <gsize.h>
#include <timekeeping.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP 100

static state prandom_testing(struct kshell_context *ctx, void *pdata)
{
    ktime_t start = timekeeping_get_time();
    unsigned int count, loop;
    uint32_t value;
    char sbuff[GSIZE_BUFF];

    for (count = 0; count < TEST_LOOP; ++count) {
        value = prandom_value();
        kshell_printf(ctx, "prandom test%02u: %#010x\n", count, value);
    }

    count = 0;
    do {
        for (loop = 0; loop < TEST_LOOP; ++loop) {
            prandom_value(); prandom_value();
            prandom_value(); prandom_value();
            prandom_value(); prandom_value();
            prandom_value(); prandom_value();
        }
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * BYTES_PER_U32 * TEST_LOOP * 8);
    kshell_printf(ctx, "prandom bandwidth: %s/s\n", sbuff);

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
