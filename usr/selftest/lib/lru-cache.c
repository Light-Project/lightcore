/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <selftest.h>
#include <lru-cache.h>

#define TEST_LOOP 100

static state lrc_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct lrc_head *head = pdata;
    unsigned int count;

    for (count = 0; count < TEST_LOOP; ++count)
        lrc_get(head, count);

    return -ENOERR;
}

static void *lrc_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return lrc_create(TEST_LOOP, TEST_LOOP, lrc_kmalloc, lrc_kfree, NULL);
}

static void lrc_test_release(struct kshell_context *ctx, void *pdata)
{
    lrc_destroy(pdata, lrc_kfree, NULL);
}

static struct selftest_command lrc_test_command = {
    .group = "lib",
    .name = "lrc",
    .desc = "lru cache unit test",
    .testing = lrc_test_testing,
    .prepare = lrc_test_prepare,
    .release = lrc_test_release,
};

static state selftest_lrc_init(void)
{
    return selftest_register(&lrc_test_command);
}
kshell_initcall(selftest_lrc_init);
