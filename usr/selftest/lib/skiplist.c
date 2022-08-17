/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <prandom.h>
#include <skiplist.h>
#include <selftest.h>

#define TEST_LOOP 100
#define TEST_LEVEL 32

struct skiplist_test {
    struct skip_head *head;
    uintptr_t values[TEST_LOOP];
};

static long skiplist_test_cmp(const void *nodea, const void *nodeb)
{
    uintptr_t valuea = (uintptr_t)nodea;
    uintptr_t valueb = (uintptr_t)nodeb;
    if (valuea == valueb) return 0;
    return valuea > valueb ? 1 : -1;
}

static state skiplist_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct skiplist_test *test = pdata;
    unsigned int count;
    uintptr_t value;
    state retval;

    skiplist_reset(test->head);
    for (count = 0; count < TEST_LOOP; ++count)
        test->values[count] = max(prandom_value(), 1);

    for (count = 0; count < TEST_LOOP; ++count) {
        retval = skiplist_insert(test->head,
                 (void *)test->values[count], skiplist_test_cmp);
        kshell_printf(ctx, "skiplist insert test%02d: %#010lx ret %d\n",
                      count, test->values[count], retval);
        if (retval)
            return retval;
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        value = (uintptr_t)skiplist_find(test->head,
                (void *)test->values[count], skiplist_test_cmp);
        kshell_printf(ctx, "skiplist find test%02d: %#010lx ret %#010lx\n",
                      count, test->values[count], value);
        if (!value)
            return -ENOENT;
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        skiplist_delete(test->head, (void *)test->values[count],
                        skiplist_test_cmp);
        kshell_printf(ctx, "skiplist delete test%02d\n", count);
    }

    return -ENOERR;
}

static void *skiplist_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct skiplist_test *test;

    test = kmalloc(sizeof(*test), GFP_KERNEL);
    if (unlikely(!test))
        return ERR_PTR(-ENOMEM);

    test->head = skiplist_create(TEST_LEVEL, GFP_KERNEL);
    if (unlikely(!test->head)) {
        kfree(test);
        return ERR_PTR(-ENOMEM);
    }

    return test;
}

static void skiplist_test_release(struct kshell_context *ctx, void *pdata)
{
    struct skiplist_test *test = pdata;
    skiplist_destroy(test->head);
    kfree(test);
}

static struct selftest_command skiplist_test_command = {
    .group = "lib",
    .name = "skiplist",
    .desc = "skiplist unit test",
    .testing = skiplist_test_testing,
    .prepare = skiplist_test_prepare,
    .release = skiplist_test_release,
};

static state selftest_skiplist_init(void)
{
    return selftest_register(&skiplist_test_command);
}
kshell_initcall(selftest_skiplist_init);
