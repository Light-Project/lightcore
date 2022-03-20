/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <memory.h>
#include <ioremap.h>
#include <string.h>
#include <random.h>
#include <selftest.h>
#include <printk.h>

#define TEST_LOOP   100
#define TEST_SIZE   SZ_1MiB

static state ioremap_testing(void *pdata)
{
    void *test_pool[TEST_LOOP];
    unsigned int count, tmp;
    state ret = -ENOERR;
    phys_addr_t phys;
    size_t size;

    for (count = 0; count < TEST_LOOP; ++count) {
        phys = page_align(clamp(random_long(), CONFIG_HIGHMEM_OFFSET,
                            PHYS_MASK - (TEST_SIZE * 2)));
        size = page_align(random_long() % TEST_SIZE + TEST_SIZE);
        printk("ioremap test%02u addr (%#lx) size (%#lx): ", count, phys, size);
        test_pool[count] = ioremap(phys, size);
        if (!test_pool[count]) {
            printk("failed\n");
            ret = -ENOMEM;
            goto error;
        }
        printk("%p\n", test_pool[count]);
    }

error:
    for (tmp = 0; tmp < count; ++tmp) {
        printk("iounmap test%02u: %p\n", tmp, test_pool[tmp]);
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
