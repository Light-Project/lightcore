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

struct crc_rocksoft_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc_rocksoft_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc_rocksoft_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc_rocksoft(cdata->mempool, SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc_rocksoft bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc_rocksoft_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc_rocksoft_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc_rocksoft_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc_rocksoft_command = {
    .group = "benchmark",
    .name = "crc-rocksoft",
    .desc = "benchmark crc-rocksoft",
    .testing = crc_rocksoft_testing,
    .prepare = crc_rocksoft_prepare,
    .release = crc_rocksoft_release,
};

static state selftest_crc_rocksoft_init(void)
{
    return selftest_register(&crc_rocksoft_command);
}
kshell_initcall(selftest_crc_rocksoft_init);
