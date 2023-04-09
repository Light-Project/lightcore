/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <mm/page.h>
#include <string.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP 100

static state buddy_testing(struct kshell_context *ctx, void *pdata)
{
    struct page *test_pool[TEST_LOOP];
    unsigned int count, tmp;
    state ret = -ENOERR;

    for (count = 0; count < TEST_LOOP; ++count) {
        tmp = (unsigned int)prandom_value() % PAGE_ORDER_MAX;
        kshell_printf(ctx, "buddy alloc test%02u order (%02u): ", count, tmp);
        test_pool[count] = page_alloc(tmp, GFP_KERNEL);
        if (!test_pool[count]) {
            kshell_puts(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        memset(page_address(test_pool[count]), 0, page_size(test_pool[count]));
        kshell_puts(ctx, "pass\n");
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        kshell_printf(ctx, "buddy free test%02u: order (%02u)\n", tmp, page_order(test_pool[tmp]));
        page_free(test_pool[tmp]);
    }

    return ret;
}

static struct selftest_command buddy_command = {
    .group = "memory",
    .name = "buddy",
    .desc = "buddy unit test",
    .testing = buddy_testing,
};

static state selftest_buddy_init(void)
{
    return selftest_register(&buddy_command);
}
kshell_initcall(selftest_buddy_init);
