/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <prandom.h>
#include <selftest.h>
#include <dynamic-call.h>

#define TEST_LOOP 100

static long dcall_test1(struct kshell_context *ctx,
                        int arg1)
{
    kshell_printf(ctx, "dcall test1: (%d) ",
        arg1);
    return arg1;
}

static long dcall_test2(struct kshell_context *ctx,
                        int arg1, int arg2)
{
    kshell_printf(ctx, "dcall test2: (%d %d) ",
        arg1, arg2);
    return arg1 + arg2;
}

static long dcall_test3(struct kshell_context *ctx,
                        int arg1, int arg2, int arg3)
{
    kshell_printf(ctx, "dcall test3: (%d %d %d) ",
        arg1, arg2, arg3);
    return arg1 + arg2 + arg3;
}

static long dcall_test4(struct kshell_context *ctx,
                        int arg1, int arg2, int arg3,
                        int arg4)
{
    kshell_printf(ctx, "dcall test4: (%d %d %d %d) ",
        arg1, arg2, arg3, arg4);
    return arg1 + arg2 + arg3 + arg4;
}

static long dcall_test5(struct kshell_context *ctx,
                        int arg1, int arg2, int arg3,
                        int arg4, int arg5)
{
    kshell_printf(ctx, "dcall test5: (%d %d %d %d %d) ",
        arg1, arg2, arg3, arg4, arg5);
    return arg1 + arg2 + arg3 + arg4 + arg5;
}

static long dcall_test6(struct kshell_context *ctx,
                        int arg1, int arg2, int arg3,
                        int arg4, int arg5, int arg6)
{
    kshell_printf(ctx, "dcall test6: (%d %d %d %d %d %d) ",
        arg1, arg2, arg3, arg4, arg5, arg6);
    return arg1 + arg2 + arg3 + arg4 + arg5 + arg6;
}

static dynamic_call_t dcall_testn[] = {
    (dynamic_call_t)dcall_test1,
    (dynamic_call_t)dcall_test2,
    (dynamic_call_t)dcall_test3,
    (dynamic_call_t)dcall_test4,
    (dynamic_call_t)dcall_test5,
    (dynamic_call_t)dcall_test6,
};

static state dcall_test_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int index, count1, count2;
    void *args[7];

    args[0] = ctx;
    for (index = 0; index < ARRAY_SIZE(dcall_testn); ++index) {
        dynamic_call_t test = dcall_testn[index];

        for (count1 = 0; count1 < TEST_LOOP; ++count1) {
            long result, addsum;

            prandom_bytes(args + 1, (index + 1) * sizeof(void *));
            result = dynamic_call(test, index + 2, args);

            for (addsum = count2 = 0; count2 <= index; ++count2)
                addsum += (long)args[count2 + 1];

            if (addsum != result) {
                kshell_puts(ctx, "failed\n");
                return -EFAULT;
            }

            kshell_puts(ctx, "pass\n");
        }
    }

    return -ENOERR;
}

static struct selftest_command dcall_test_command = {
    .group = "lib",
    .name = "dcall",
    .desc = "dynamic call unit test",
    .testing = dcall_test_testing,
};

static state selftest_dcall_init(void)
{
    return selftest_register(&dcall_test_command);
}
kshell_initcall(selftest_dcall_init);
