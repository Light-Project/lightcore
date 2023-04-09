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

struct crc_itut_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state crc_itut_testing(struct kshell_context *ctx, void *pdata)
{
    struct crc_itut_pdata *cdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[GSIZE_BUFF];

    do {
        crc_itut(cdata->mempool, SZ_1MiB, 0);
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    kshell_printf(ctx, "crc_itut bandwidth: %s/s\n", sbuff);

    return -ENOERR;
}

static void *crc_itut_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct crc_itut_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    prandom_bytes(pdata->mempool, SZ_1MiB);
    return pdata;
}

static void crc_itut_release(struct kshell_context *ctx, void *pdata)
{
    vfree(pdata);
}

static struct selftest_command crc_itut_command = {
    .group = "benchmark",
    .name = "crc-itut",
    .desc = "benchmark crc-itut",
    .testing = crc_itut_testing,
    .prepare = crc_itut_prepare,
    .release = crc_itut_release,
};

static state selftest_crc_itut_init(void)
{
    return selftest_register(&crc_itut_command);
}
kshell_initcall(selftest_crc_itut_init);
