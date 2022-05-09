/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <string.h>
#include <random.h>
#include <selftest.h>

#define TEST_SIZE1      128
#define TEST_SIZE2      256
#define TEST_SIZE3      512

#define TEST_LOOP1      100
#define TEST_LOOP2      100
#define TEST_LOOP3      100

static state kmalloc_testing(struct kshell_context *ctx, void *pdata)
{
    void *test_pool1[TEST_LOOP1];
    void *test_pool2[TEST_LOOP2];
    void *test_pool3[TEST_LOOP3];
    unsigned int count1;
    unsigned int count2;
    unsigned int count3;
    unsigned int count;
    state ret = -ENOERR;

    for (count1 = 0; count1 < TEST_LOOP1; count1++) {
        test_pool1[count1] = kmalloc_align(
            TEST_SIZE1 + (unsigned int)random_long() % TEST_SIZE1,
            GFP_KERNEL,  (unsigned int)random_long() % TEST_SIZE1
        );
        kshell_printf(ctx, "kmalloc alloc size ("__stringify(TEST_SIZE1)") test%02u: ", count1);
        if (!test_pool1[count1]) {
            kshell_printf(ctx, "failed\n");
            ret = -ENOMEM;
            goto err_p1;
        }
        memset(test_pool1[count1], 0, TEST_SIZE1);
        kshell_printf(ctx, "pass\n");
    }

    for (count2 = 0; count2 < TEST_LOOP2; count2++) {
        test_pool2[count2] = kmalloc_align(
            TEST_SIZE2 + (unsigned int)random_long() % TEST_SIZE2,
            GFP_KERNEL,  (unsigned int)random_long() % TEST_SIZE2
        );
        kshell_printf(ctx, "kmalloc alloc size ("__stringify(TEST_SIZE2)") test%02u: ", count2);
        if (!test_pool2[count2]) {
            kshell_printf(ctx, "failed\n");
            ret = -ENOMEM;
            goto err_p2;
        }
        memset(test_pool2[count2], 0, TEST_SIZE2);
        kshell_printf(ctx, "pass\n");
    }

    for (count3 = 0; count3 < TEST_LOOP3; count3++) {
        test_pool3[count3] = kmalloc_align(
            TEST_SIZE3 + (unsigned int)random_long() % TEST_SIZE3,
            GFP_KERNEL,  (unsigned int)random_long() % TEST_SIZE3
        );
        kshell_printf(ctx, "kmalloc alloc size ("__stringify(TEST_SIZE3)") test%02u: ", count3);
        if (!test_pool3[count3]) {
            kshell_printf(ctx, "failed\n");
            ret = -ENOMEM;
            goto err_p3;
        }
        memset(test_pool3[count3], 0, TEST_SIZE3);
        kshell_printf(ctx, "pass\n");
    }

err_p3:
    for (count = 0; count < count3; ++count) {
        kshell_printf(ctx, "kmalloc free size ("__stringify(TEST_SIZE3)") test%02u\n", count);
        kfree(test_pool3[count]);
    }

err_p2:
    for (count = 0; count < count2; ++count) {
        kshell_printf(ctx, "kmalloc free size ("__stringify(TEST_SIZE2)") test%02u\n", count);
        kfree(test_pool2[count]);
    }

err_p1:
    for (count = 0; count < count1; ++count) {
        kshell_printf(ctx, "kmalloc free size ("__stringify(TEST_SIZE1)") test%02u\n", count);
        kfree(test_pool1[count]);
    }

    return ret;
}

static struct selftest_command kmalloc_command = {
    .group = "memory",
    .name = "kmalloc",
    .desc = "kmalloc unit test",
    .testing = kmalloc_testing,
};

static state selftest_kmalloc_init(void)
{
    return selftest_register(&kmalloc_command);
}
kshell_initcall(selftest_kmalloc_init);
