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

struct strncmp_pdata {
    char mempool_a[SZ_1MiB];
    char mempool_b[SZ_1MiB];
};

static state strncmp_testing(struct kshell_context *ctx, void *pdata)
{
    struct strncmp_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (strncmp(mdata->mempool_b, mdata->mempool_a, SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "strncmp bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *strncmp_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct strncmp_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB; ++count)
        pdata->mempool_a[count] =
        pdata->mempool_b[count] = count % UINT8_MAX | 1;

    return pdata;
}

static void strncmp_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command strncmp_command = {
    .group = "benchmark",
    .name = "strncmp",
    .desc = "benchmark strncmp",
    .testing = strncmp_testing,
    .prepare = strncmp_prepare,
    .release = strncmp_release,
};

static state selftest_strncmp_init(void)
{
    return selftest_register(&strncmp_command);
}
kshell_initcall(selftest_strncmp_init);
