/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <gunit.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <crypto/crc.h>
#include <prandom.h>
#include <selftest.h>

struct crc64_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc64_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc64_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc64(cdata->mempool, SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc64 bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc64_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc64_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc64_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc64_command = {
    .group = "benchmark",
    .name = "crc64",
    .desc = "benchmark crc64",
    .testing = crc64_testing,
    .prepare = crc64_prepare,
    .release = crc64_release,
};

static state selftest_crc64_init(void)
{
    return selftest_register(&crc64_command);
}
kshell_initcall(selftest_crc64_init);
