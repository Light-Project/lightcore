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

struct crc8_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc8_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc8_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc8(cdata->mempool, SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc8 bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc8_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc8_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc8_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc8_command = {
    .group = "benchmark",
    .name = "crc8",
    .desc = "benchmark crc8",
    .testing = crc8_testing,
    .prepare = crc8_prepare,
    .release = crc8_release,
};

static state selftest_crc8_init(void)
{
    return selftest_register(&crc8_command);
}
kshell_initcall(selftest_crc8_init);
