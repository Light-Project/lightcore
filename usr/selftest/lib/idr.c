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

static state idr_testing(void *pdata)
{
    struct idr idr = IDR_INIT;
    ktime_t start = timekeeping_get_time();
    unsigned long walk, count;
    unsigned long ida[TEST_LOOP];

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_alloc(&idr, NULL, GFP_KERNEL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_free(&idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    printk("idr sequential: %lu/s\n", count);

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_alloc_cyclic(&idr, NULL, GFP_KERNEL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_free(&idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    printk("idr cyclic: %lu/s\n", count);

    idr_destroy(&idr);
    return -ENOERR;
}

static struct selftest_command idr_command = {
    .group = "lib",
    .name = "idr",
    .desc = "benchmark idr",
    .testing = idr_testing,
};

static state selftest_idr_init(void)
{
    return selftest_register(&idr_command);
}
kshell_initcall(selftest_idr_init);
