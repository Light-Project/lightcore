/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gsize.h>
#include <initcall.h>
#include <mm/vmem.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP   100
#define TEST_SIZE   SZ_1MiB

static state vmem_testing(struct kshell_context *ctx, void *pdata)
{
    struct vmem_area *test_pool[TEST_LOOP];
    char sbuff[GSIZE_BUFF];
    unsigned int count, tmp;
    state ret = -ENOERR;
    size_t size;

    for (count = 0; count < TEST_LOOP; ++count) {
        size = ((unsigned int)prandom_value() % TEST_SIZE) + TEST_SIZE;
        gsize(sbuff, size);
        kshell_printf(ctx, "vmem alloc test%02u size (%s): ", count, sbuff);
        test_pool[count] = vmem_alloc(size);
        if (!test_pool[count]) {
            kshell_printf(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        kshell_printf(ctx, "pass\n");
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        kshell_printf(ctx, "vmem free test%02u\n", tmp);
        vmem_free(test_pool[tmp]);
    }

    return ret;
}

static struct selftest_command vmem_command = {
    .group = "memory",
    .name = "vmem",
    .desc = "vmem unit test",
    .testing = vmem_testing,
};

static state selftest_vmem_init(void)
{
    return selftest_register(&vmem_command);
}
kshell_initcall(selftest_vmem_init);
