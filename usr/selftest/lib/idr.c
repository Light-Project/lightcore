/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <kmalloc.h>
#include <idr.h>
#include <selftest.h>

#define TEST_LOOP 100

struct idr_test_pdata {
    struct idr_root *idr;
    struct idr_node node[TEST_LOOP];
};

static state idr_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct idr_test_pdata *idata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned long walk, count;
    unsigned long ida[TEST_LOOP];

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_node_alloc(idata->idr, &idata->node[walk], NULL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_node_free(idata->idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    kshell_printf(ctx, "idr sequential: %lu/s\n", count * TEST_LOOP);

    count = 0;
    do {
        for (walk = 0; walk < TEST_LOOP; ++walk)
            ida[walk] = idr_node_alloc_cyclic(idata->idr, &idata->node[walk], NULL);
        for (walk = 0; walk < TEST_LOOP; ++walk)
            idr_node_free(idata->idr, ida[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 2000)
    ));

    kshell_printf(ctx, "idr cyclic: %lu/s\n", count * TEST_LOOP);

    return -ENOERR;
}

static void *idr_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct idr_test_pdata *idata;

    idata = kmalloc(sizeof(*idata), GFP_KERNEL);
    if (!idata)
        return NULL;

    idata->idr = idr_create(0);
    if (!idata->idr)
        return NULL;

    return idata;
}

static void idr_test_release(struct kshell_context *ctx, void *pdata)
{
    struct idr_test_pdata *idata = pdata;
    idr_delete(idata->idr);
    kfree(idata);
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
