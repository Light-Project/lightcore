/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <mempool.h>
#include <string.h>
#include <random.h>
#include <selftest.h>

#define TEST_LOOP   100
#define TEST_SIZE   SZ_32KiB

static state mempool_testing(struct kshell_context *ctx, void *pdata)
{
    void *test_pool[TEST_LOOP];
    unsigned int count, tmp;
    struct mempool *pool;
    state ret = -ENOERR;

    pool = mempool_create(TEST_LOOP / 2, TEST_SIZE, GFP_KERNEL);
    if (!pool)
        return -ENOMEM;

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "mempool alloc test%02u: ", count);
        test_pool[count] = mempool_alloc(pool, GFP_KERNEL);
        if (!test_pool[count]) {
            kshell_puts(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        memset(test_pool[count], 0, TEST_SIZE);
        kshell_puts(ctx, "pass\n");
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        kshell_printf(ctx, "mempool free test%02u\n", tmp);
        mempool_free(pool, test_pool[tmp]);
    }

    return ret;
}

static struct selftest_command mempool_command = {
    .group = "memory",
    .name = "mempool",
    .desc = "mempool unit test",
    .testing = mempool_testing,
};

static state selftest_mempool_init(void)
{
    return selftest_register(&mempool_command);
}
kshell_initcall(selftest_mempool_init);
