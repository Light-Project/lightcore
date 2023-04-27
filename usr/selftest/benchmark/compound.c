/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <gunit.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <selftest.h>

struct compound_pdata {
    uint8_t mempool_a[SZ_1MiB];
    uint8_t mempool_b[SZ_1MiB];
};

static state compound_testing(struct kshell_context *ctx, void *pdata)
{
    struct compound_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        *mdata->mempool_b = *mdata->mempool_a;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "compound bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *compound_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct compound_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool_a[count] = count % UINT8_MAX;

    return pdata;
}

static void compound_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command compound_command = {
    .group = "benchmark",
    .name = "compound literals",
    .desc = "benchmark compound literals",
    .testing = compound_testing,
    .prepare = compound_prepare,
    .release = compound_release,
};

static state selftest_compound_init(void)
{
    return selftest_register(&compound_command);
}
kshell_initcall(selftest_compound_init);
