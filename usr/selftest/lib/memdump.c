/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <printk.h>
#include <selftest.h>

static const uint64_t memdump_data[] = {
    0x55aa55aa55aa55aa, 0xaa55aa55aa55aa55,
    0x0123456789abcdef, 0xfedcba9876543210,
};

static state memdump_test_testing(void *pdata)
{
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_NONE, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_OFFSET, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 1, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 2, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 4, 16, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 8, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 10, true);
    memdump_print("", "memdump-test: ", MEMDUMP_PREFIX_ADDRESS, memdump_data, sizeof(memdump_data), 16, 8, 16, true);

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
