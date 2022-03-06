/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <kmalloc.h>
#include <idr.h>
#include <selftest.h>
#include <printk.h>

#define TEST_LOOP 100

static state idr_test_testing(void *pdata)
{
    struct idr *idr = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned long walk, count;
    unsigned long ida[TEST_LOOP];

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_alloc(idr, NULL, GFP_KERNEL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_free(idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    printk("idr sequential: %lu/s\n", count * TEST_LOOP);

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_alloc_cyclic(idr, NULL, GFP_KERNEL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_free(idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 2000)
    ));

    printk("idr cyclic: %lu/s\n", count * TEST_LOOP);

    return -ENOERR;
}

static void *idr_test_prepare(int argc, char *argv[])
{
    return idr_create(0);
}

static void idr_test_release(void *pdata)
{
    idr_delete(pdata);
}

static struct selftest_command idr_command = {
    .group = "lib",
    .name = "idr",
    .desc = "benchmark idr",
    .testing = idr_test_testing,
    .prepare = idr_test_prepare,
    .release = idr_test_release,
};

static state selftest_idr_init(void)
{
    return selftest_register(&idr_command);
}
kshell_initcall(selftest_idr_init);
