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

struct strnlen_pdata {
    char mempool[SZ_1MiB];
};

static state strnlen_testing(struct kshell_context *ctx, void *pdata)
{
    struct strnlen_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (strnlen(mdata->mempool, SZ_1MiB) != SZ_1MiB)
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "strnlen bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *strnlen_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct strnlen_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool[count] = count % UINT8_MAX | 1;

    return pdata;
}

static void strnlen_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command strnlen_command = {
    .group = "benchmark",
    .name = "strnlen",
    .desc = "benchmark strnlen",
    .testing = strnlen_testing,
    .prepare = strnlen_prepare,
    .release = strnlen_release,
};

static state selftest_strnlen_init(void)
{
    return selftest_register(&strnlen_command);
}
kshell_initcall(selftest_strnlen_init);