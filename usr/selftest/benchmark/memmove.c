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

struct memmove_pdata {
    uint8_t mempool_a[SZ_1MiB];
    uint8_t mempool_b[SZ_1MiB];
};

static state memmove_testing(struct kshell_context *ctx, void *pdata)
{
    struct memmove_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        memmove(mdata->mempool_b, mdata->mempool_a, SZ_1MiB);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memmove bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memmove_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memmove_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    return pdata;
}

static void memmove_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memmove_command = {
    .group = "benchmark",
    .name = "memmove",
    .desc = "benchmark memmove",
    .testing = memmove_testing,
    .prepare = memmove_prepare,
    .release = memmove_release,
};

static state selftest_memmove_init(void)
{
    return selftest_register(&memmove_command);
}
kshell_initcall(selftest_memmove_init);
