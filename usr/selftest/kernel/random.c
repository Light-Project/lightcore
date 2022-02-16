/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <random.h>
#include <selftest.h>
#include <printk.h>

#define TEST_LOOP 100

static state random_testing(void *pdata)
{
    unsigned int count;
    long value;

    for (count = 0; count < TEST_LOOP; ++count) {
        value = random_long();
        printk("test%02u: %lx\n", count, value);
    }

    return -ENOERR;
}

static struct selftest_command random_command = {
    .group = "kernel",
    .name = "random",
    .desc = "benchmark random",
    .testing = random_testing,
};

static state selftest_random_init(void)
{
    return selftest_register(&random_command);
}
kshell_initcall(selftest_random_init);
