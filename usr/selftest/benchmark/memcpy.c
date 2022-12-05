/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gunit.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <string.h>
#include <selftest.h>

struct memcpy_pdata {
    uint8_t mempool_a[SZ_1MiB];
    uint8_t mempool_b[SZ_1MiB];
};

static state memcpy_testing(struct kshell_context *ctx, void *pdata)
{
    struct memcpy_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        memcpy(mdata->mempool_b, mdata->mempool_a, SZ_1MiB);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memcpy bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memcpy_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memcpy_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool_a[count] = count % UINT8_MAX;

    return pdata;
}

static void memcpy_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memcpy_command = {
    .group = "benchmark",
    .name = "memcpy",
    .desc = "benchmark memcpy",
    .testing = memcpy_testing,
    .prepare = memcpy_prepare,
    .release = memcpy_release,
};

static state selftest_memcpy_init(void)
{
    return selftest_register(&memcpy_command);
}
kshell_initcall(selftest_memcpy_init);
