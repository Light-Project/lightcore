/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <prandom.h>
#include <math.h>
#include <selftest.h>

#define TEST_LOOP 100

static state bezier3_testing(struct kshell_context *ctx, void *pdata)
{
    uint32_t points[4], count;
    uint32_t value;

    prandom_bytes(points, sizeof(points));
    for (count = 0; count < ARRAY_SIZE(points); ++count) {
        value = (points[count] %= BEZIER_VALUE_MAX);
        kshell_printf(ctx, "bezier3 point%u: %u\n", count, value);
    }

    for (count = 0; count < BEZIER_VALUE_MAX; count += BEZIER_VALUE_MAX / TEST_LOOP) {
        value = bezier3(count, points[0], points[1], points[2], points[3]);
        kshell_printf(ctx, "bezier3 test%u: %u\n", count, value);
    }

    return -ENOERR;
}

static struct selftest_command bezier3_command = {
    .group = "math",
    .name = "bezier3",
    .testing = bezier3_testing,
};

static state selftest_bezier3_init(void)
{
    return selftest_register(&bezier3_command);
}
kshell_initcall(selftest_bezier3_init);
