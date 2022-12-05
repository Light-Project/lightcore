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

struct memcmp_pdata {
    uint8_t mempool_a[SZ_1MiB];
    uint8_t mempool_b[SZ_1MiB];
};

static state memcmp_testing(struct kshell_context *ctx, void *pdata)
{
    struct memcmp_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (memcmp(mdata->mempool_b, mdata->mempool_a, SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memcmp bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memcmp_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memcmp_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool_a[count] =
        pdata->mempool_b[count] = count % UINT8_MAX;

    return pdata;
}

static void memcmp_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memcmp_command = {
    .group = "benchmark",
    .name = "memcmp",
    .desc = "benchmark memcmp",
    .testing = memcmp_testing,
    .prepare = memcmp_prepare,
    .release = memcmp_release,
};

static state selftest_memcmp_init(void)
{
    return selftest_register(&memcmp_command);
}
kshell_initcall(selftest_memcmp_init);
