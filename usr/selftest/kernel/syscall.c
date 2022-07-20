/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <syscall.h>
#include <initcall.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP 100

DEFINE_SYSCALL6(selftest, int, arg1, int, arg2, int, arg3,
                int, arg4, int, arg5, int, arg6)
{
    return arg1 + arg2 + arg3 + arg4 + arg5 + arg6;
}

static state syscall_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;
    int result, args[6];

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "softirq test%02u: ", count);
        prandom_bytes(args, sizeof(args));
        result = lightcore_selftest(args[0], args[1], args[2],
                                    args[3], args[4], args[5]);
        if (result != args[0] + args[1] + args[2] +
                      args[3] + args[4] + args[5]) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    return -ENOERR;
}

static struct selftest_command syscall_command = {
    .group = "kernel",
    .name = "syscall",
    .desc = "syscall unit test",
    .testing = syscall_testing,
};

static state selftest_syscall_init(void)
{
    return selftest_register(&syscall_command);
}
kshell_initcall(selftest_syscall_init);
