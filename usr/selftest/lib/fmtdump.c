/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <fmtdump.h>
#include <prandom.h>
#include <selftest.h>

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

static uint8_t test_read8(void *pdata, uintptr_t addr)
{
    uint8_t value;
    prandom_bytes(&value, sizeof(value));
    return value;
}

static uint16_t test_read16(void *pdata, uintptr_t addr)
{
    uint16_t value;
    prandom_bytes(&value, sizeof(value));
    return value;
}

static uint32_t test_read32(void *pdata, uintptr_t addr)
{
    uint32_t value;
    prandom_bytes(&value, sizeof(value));
    return value;
}

static uint64_t test_read64(void *pdata, uintptr_t addr)
{
    uint64_t value;
    prandom_bytes(&value, sizeof(value));
    return value;
}

static struct fmtdump_ops test_ops = {
    .print = test_print,
    .read8 = test_read8,
    .read16 = test_read16,
    .read32 = test_read32,
    .read64 = test_read64,
};

static state fmtdump_test_testing(struct kshell_context *ctx, void *pdata)
{
    kshell_printf(ctx, "fmtdump-test mode: none, group: 1, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 1, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 1, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 1, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 1, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 1, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 2, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 2, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 2, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 2, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 2, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 2, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 4, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 4, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 4, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 4, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 4, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 4, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 8, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 8, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 8, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 8, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: none, group: 8, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_NONE, 0, 64, 16, 8, 16, true);

    kshell_printf(ctx, "fmtdump-test mode: offset, group: 1, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 1, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 1, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 1, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 1, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 1, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 2, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 2, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 2, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 2, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 2, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 2, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 4, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 4, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 4, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 4, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 4, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 4, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 8, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 8, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 8, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 8, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: offset, group: 8, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_OFFSET, 0, 64, 16, 8, 16, true);

    kshell_printf(ctx, "fmtdump-test mode: address, group: 1, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 1, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 1, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 1, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 1, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 1, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 2, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 2, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 2, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 2, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 2, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 2, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 4, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 4, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 4, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 4, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 4, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 4, 16, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 8, base: 8\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 8, 8, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 8, base: 10\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 8, 10, true);
    kshell_printf(ctx, "fmtdump-test mode: address, group: 8, base: 16\n");
    fmtdump_print(&test_ops, ctx, "# ", FMTDUMP_PREFIX_ADDRESS, 0x80000000, 64, 16, 8, 16, true);

    return -ENOERR;
}

static struct selftest_command bitops_command = {
    .group = "lib",
    .name = "fmtdump",
    .desc = "fmtdump unit test",
    .testing = fmtdump_test_testing,
};

static state selftest_bitops_init(void)
{
    return selftest_register(&bitops_command);
}
kshell_initcall(selftest_bitops_init);
