/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <gunit.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <crypto/crc.h>
#include <prandom.h>
#include <selftest.h>

struct crc32_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc32_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc32_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc32(cdata->mempool, SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc32 bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc32_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc32_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc32_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc32_command = {
    .group = "benchmark",
    .name = "crc32",
    .desc = "benchmark crc32",
    .testing = crc32_testing,
    .prepare = crc32_prepare,
    .release = crc32_release,
};

static state selftest_crc32_init(void)
{
    return selftest_register(&crc32_command);
}
kshell_initcall(selftest_crc32_init);
