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

struct memchr_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state memchr_testing(struct kshell_context *ctx, void *pdata)
{
    struct memchr_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (memchr(mdata->mempool, 0xff, SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "memchr bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *memchr_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct memchr_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    memset(pdata->mempool, 0, SZ_1MiB);
    return pdata;
}

static void memchr_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memchr_command = {
    .group = "benchmark",
    .name = "memchr",
    .desc = "benchmark memchr",
    .testing = memchr_testing,
    .prepare = memchr_prepare,
    .release = memchr_release,
};

static state selftest_memchr_init(void)
{
    return selftest_register(&memchr_command);
}
kshell_initcall(selftest_memchr_init);
