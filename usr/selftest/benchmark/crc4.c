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

struct crc4_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc4_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc4_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc4(cdata->mempool, BITS_PER_U8 * SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc4 bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc4_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc4_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc4_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc4_command = {
    .group = "benchmark",
    .name = "crc4",
    .desc = "benchmark crc4",
    .testing = crc4_testing,
    .prepare = crc4_prepare,
    .release = crc4_release,
};

static state selftest_crc4_init(void)
{
    return selftest_register(&crc4_command);
}
kshell_initcall(selftest_crc4_init);
