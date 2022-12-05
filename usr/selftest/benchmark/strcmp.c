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

struct strcmp_pdata {
    char mempool_a[SZ_1MiB];
    char mempool_b[SZ_1MiB];
};

static state strcmp_testing(struct kshell_context *ctx, void *pdata)
{
    struct strcmp_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        if (strcmp(mdata->mempool_b, mdata->mempool_a))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "strcmp bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *strcmp_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct strcmp_pdata *pdata;
    unsigned int count;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    for (count = 0; count < SZ_1MiB - 1; ++count)
        pdata->mempool_a[count] =
        pdata->mempool_b[count] = count % UINT8_MAX | 1;

    pdata->mempool_a[SZ_1MiB - 1] =
    pdata->mempool_b[SZ_1MiB - 1] = '\0';

    return pdata;
}

static void strcmp_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command strcmp_command = {
    .group = "benchmark",
    .name = "strcmp",
    .desc = "benchmark strcmp",
    .testing = strcmp_testing,
    .prepare = strcmp_prepare,
    .release = strcmp_release,
};

static state selftest_strcmp_init(void)
{
    return selftest_register(&strcmp_command);
}
kshell_initcall(selftest_strcmp_init);
