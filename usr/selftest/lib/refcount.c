/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <kmalloc.h>
#include <refcount.h>
#include <selftest.h>

#define TEST_LOOP 100

static state refcount_testing(void *pdata)
{
    struct refcount ref;
    unsigned int count, tmp;

    refco
    for (count = 0; count < TEST_LOOP; ++count) {



    }



    return -ENOERR;
}

static struct selftest_command refcount_command = {
    .group = "lib",
    .name = "refcount",
    .desc = " refcount unit test",
    .testing = refcount_testing,
};

static state selftest_refcount_init(void)
{
    return selftest_register(&refcount_command);
}
kshell_initcall(selftest_refcount_init);
