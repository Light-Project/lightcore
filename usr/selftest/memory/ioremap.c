/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <memory.h>
#include <ioremap.h>
#include <string.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP   100
#define TEST_SIZE   SZ_1MiB

static state ioremap_testing(struct kshell_context *ctx, void *pdata)
{
    void *test_pool[TEST_LOOP];
    unsigned int count, tmp;
    state ret = -ENOERR;
    phys_addr_t phys;
    size_t size;

    for (count = 0; count < TEST_LOOP; ++count) {
        phys = page_align(clamp(prandom_value(), CONFIG_HIGHMEM_OFFSET,
                            PHYS_MASK - (TEST_SIZE * 2)));
        size = page_align(prandom_value() % TEST_SIZE + TEST_SIZE);
        kshell_printf(ctx, "ioremap test%02u addr (%#lx) size (%#lx): ", count, phys, size);
        test_pool[count] = ioremap(phys, size);
        if (!test_pool[count]) {
            kshell_puts(ctx, "failed\n");
            ret = -ENOMEM;
            goto error;
        }
        kshell_printf(ctx, "%p\n", test_pool[count]);
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        kshell_printf(ctx, "iounmap test%02u: %p\n", tmp, test_pool[tmp]);
        iounmap(test_pool[tmp]);
    }

    return ret;
}

static struct selftest_command ioremap_command = {
    .group = "memory",
    .name = "ioremap",
    .desc = "ioremap unit test",
    .testing = ioremap_testing,
};

static state selftest_ioremap_init(void)
{
    return selftest_register(&ioremap_command);
}
kshell_initcall(selftest_ioremap_init);
