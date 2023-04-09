/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <string.h>
#include <random.h>
#include <selftest.h>

#define TEST_LOOP   100
#define TEST_SIZE   128

static state kcache_testing(struct kshell_context *ctx, void *pdata)
{
    void *test_pool[TEST_LOOP];
    unsigned int count;
    struct kcache *cache;
    state ret = -ENOERR;

    cache = kcache_create("kcache-test", TEST_SIZE, 0);
    if (!cache) {
        kshell_puts(ctx, "kcache create failed\n");
        return -ENOMEM;
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        test_pool[count] = kcache_alloc(cache, GFP_KERNEL);
        kshell_printf(ctx, "kcache alloc size ("__stringify(TEST_SIZE)") test%02u: ", count);
        if (!test_pool[count]) {
            kshell_puts(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        memset(test_pool[count], 0, TEST_SIZE);
        kshell_puts(ctx, "pass\n");
    }

error:
    kcache_delete(cache);
    return ret;
}

static struct selftest_command kcache_command = {
    .group = "memory",
    .name = "kcache",
    .desc = "kcache unit test",
    .testing = kcache_testing,
};

static state selftest_kcache_init(void)
{
    return selftest_register(&kcache_command);
}
kshell_initcall(selftest_kcache_init);
