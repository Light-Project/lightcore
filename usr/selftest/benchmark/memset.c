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

struct memset_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state memset_testing(struct kshell_context *ctx, void *pdata)
{
    struct memset_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        memset(mdata->mempool, 0, SZ_1MiB);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memset bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memset_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memset_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    return pdata;
}

static void memset_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memset_command = {
    .group = "benchmark",
    .name = "memset",
    .desc = "benchmark memset",
    .testing = memset_testing,
    .prepare = memset_prepare,
    .release = memset_release,
};

static state selftest_memset_init(void)
{
    return selftest_register(&memset_command);
}
kshell_initcall(selftest_memset_init);
