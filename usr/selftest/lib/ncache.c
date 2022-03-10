/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <kmalloc.h>
#include <ncache.h>
#include <selftest.h>
#include <printk.h>

#define TEST_DEEPTH 100

static void *ncache_node_alloc(void *pdata)
{
    return ncache_node_alloc;
}

static void ncache_node_free(void *object, void *pdata)
{
    /* Nothing */
}

static state ncache_testing(void *pdata)
{
    struct ncache *ndata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned long walk, count;
    void *object[TEST_DEEPTH];

    count = 0;
    do {
        object[0] = ncache_get(ndata);
        ncache_put(ndata, object[0]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    printk("ncache interlaced: %lu/s\n", count);

    count = 0;
    do {
        for (walk = 0; walk < TEST_DEEPTH; ++walk)
            object[walk] = ncache_get(ndata);
        for (walk = 0; walk < TEST_DEEPTH; ++walk)
            ncache_put(ndata, object[walk]);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 2000)
    ));

    printk("ncache multilayer: %lu/s\n", count * TEST_DEEPTH);

    return -ENOERR;
}

static void *ncache_prepare(int argc, char *argv[])
{
    struct ncache *pdata;

    pdata = ncache_create(ncache_node_alloc, ncache_node_free, TEST_DEEPTH, NCACHE_NOFULL, NULL);
    if (!pdata)
        return ERR_PTR(-ENOERR);

    return pdata;
}

static void ncache_release(void *pdata)
{
    ncache_destroy(pdata);
}

static struct selftest_command ncache_command = {
    .group = "lib",
    .name = "ncache",
    .desc = "benchmark ncache",
    .testing = ncache_testing,
    .prepare = ncache_prepare,
    .release = ncache_release,
};

static state selftest_ncache_init(void)
{
    return selftest_register(&ncache_command);
}
kshell_initcall(selftest_ncache_init);
