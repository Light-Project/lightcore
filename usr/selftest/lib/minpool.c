/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <vmalloc.h>
#include <minpool.h>
#include <prandom.h>
#include <selftest.h>

#define POOL_SIZE SZ_128KiB
#define TEST_SIZE SZ_64KiB
#define TEST_LOOP 64

struct minpool_test {
    uint8_t pool[POOL_SIZE];
    void *result[TEST_LOOP];
};

static state minpool_testing(struct kshell_context *ctx, void *pdata)
{
    struct minpool_test *mtest = pdata;
    struct minpool_head minpool_test;
    unsigned int count, index;
    size_t size;

    minpool_setup(&minpool_test, &mtest->pool, POOL_SIZE);
    for (count = 0; count < TEST_LOOP; ++count) {
        size = prandom_value() % (TEST_SIZE / TEST_LOOP);
        mtest->result[count] = minpool_alloc(&minpool_test, size);
        kshell_printf(ctx, "minpool random alloc%02u: %p\n",
                      count, mtest->result[count]);
        if (!mtest->result[count])
            return -ENOMEM;

        size = prandom_value() % (TEST_SIZE / TEST_LOOP);
        mtest->result[count] = minpool_realloc(&minpool_test, mtest->result[count], size);
        kshell_printf(ctx, "minpool random realloc%02u: %p\n",
                      count, mtest->result[count]);
        if (!mtest->result[count])
            return -ENOMEM;
    }

    for (count = 0; count < TEST_LOOP;) {
        index = prandom_value() % TEST_LOOP;
        if (mtest->result[index]) {
            kshell_printf(ctx, "minpool random free%02d: %p %d\n",
                          count, mtest->result[index], index);
            minpool_free(&minpool_test, mtest->result[index]);
            mtest->result[index] = NULL;
            count++;
        }
    }

    return -ENOERR;
}

static void *minpool_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return vmalloc(sizeof(struct minpool_test));
}

static void minpool_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command minpool_command = {
    .group = "lib",
    .name = "minpool",
    .desc = "benchmark minpool",
    .testing = minpool_testing,
    .prepare = minpool_prepare,
    .release = minpool_release,
};

static state selftest_minpool_init(void)
{
    return selftest_register(&minpool_command);
}
kshell_initcall(selftest_minpool_init);
