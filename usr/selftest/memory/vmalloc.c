/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <vmalloc.h>
#include <string.h>
#include <random.h>
#include <selftest.h>

#define TEST_LOOP   100
#define TEST_SIZE   SZ_1MiB

static state vmalloc_testing(struct kshell_context *ctx, void *pdata)
{
    void *test_pool[TEST_LOOP];
    char sbuff[32];
    unsigned int count, tmp;
    state ret = -ENOERR;
    size_t size;

    for (count = 0; count < TEST_LOOP; ++count) {
        size = ((unsigned int)random_long() % TEST_SIZE) + TEST_SIZE;
        gsize(sbuff, size);
        kshell_printf(ctx, "vmalloc alloc test%02u size (%s): ", count, sbuff);
        test_pool[count] = vmalloc(size);
        if (!test_pool[count]) {
            kshell_printf(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        memset(test_pool[count], 0, size);
        kshell_printf(ctx, "pass\n");
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        kshell_printf(ctx, "vmalloc free test%02u\n", tmp);
        vfree(test_pool[tmp]);
    }

    return ret;
}

static struct selftest_command vmalloc_command = {
    .group = "memory",
    .name = "vmalloc",
    .desc = "vmalloc unit test",
    .testing = vmalloc_testing,
};

static state selftest_vmalloc_init(void)
{
    return selftest_register(&vmalloc_command);
}
kshell_initcall(selftest_vmalloc_init);
