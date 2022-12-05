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

struct memscan_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state memscan_testing(struct kshell_context *ctx, void *pdata)
{
    struct memscan_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (memscan(mdata->mempool, 0xff, SZ_1MiB)
            != (mdata->mempool + SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memscan bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memscan_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memscan_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    memset(pdata->mempool, 0, SZ_1MiB);
    return pdata;
}

static void memscan_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memscan_command = {
    .group = "benchmark",
    .name = "memscan",
    .desc = "benchmark memscan",
    .testing = memscan_testing,
    .prepare = memscan_prepare,
    .release = memscan_release,
};

static state selftest_memscan_init(void)
{
    return selftest_register(&memscan_command);
}
kshell_initcall(selftest_memscan_init);
