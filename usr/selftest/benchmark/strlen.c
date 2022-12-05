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

struct strlen_pdata {
    char mempool[SZ_1MiB];
};

static state strlen_testing(struct kshell_context *ctx, void *pdata)
{
    struct strlen_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (strlen(mdata->mempool) != SZ_1MiB - 1)
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "strlen bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *strlen_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct strlen_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB - 1; ++count)
        pdata->mempool[count] = count % UINT8_MAX | 1;

    pdata->mempool[SZ_1MiB - 1] = '\0';

    return pdata;
}

static void strlen_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command strlen_command = {
    .group = "benchmark",
    .name = "strlen",
    .desc = "benchmark strlen",
    .testing = strlen_testing,
    .prepare = strlen_prepare,
    .release = strlen_release,
};

static state selftest_strlen_init(void)
{
    return selftest_register(&strlen_command);
}
kshell_initcall(selftest_strlen_init);
