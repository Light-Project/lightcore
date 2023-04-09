/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <memdump.h>
#include <selftest.h>

static const uint64_t memdump_data[] = {
    0x55aa55aa55aa55aa, 0xaa55aa55aa55aa55,
    0x0123456789abcdef, 0xfedcba9876543210,
};

static int test_print(void *pdata, const char *fmt, ...)
{
    struct kshell_context *ctx = pdata;
    va_list args;
    int len;

    va_start(args, fmt);
    len = kshell_vprintf(ctx, fmt, args);
    va_end(args);

    return len;
}

static state memdump_test_testing(struct kshell_context *ctx, void *pdata)
{
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print(test_print, ctx, "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

    return -ENOERR;
}

static struct selftest_command bitops_command = {
    .group = "lib",
    .name = "memdump",
    .desc = "memdump unit test",
    .testing = memdump_test_testing,
};

static state selftest_bitops_init(void)
{
    return selftest_register(&bitops_command);
}
kshell_initcall(selftest_bitops_init);
