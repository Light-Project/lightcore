/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <timekeeping.h>
#include <vmalloc.h>
#include <string.h>
#include <selftest.h>
#include <printk.h>

struct memchr_pdata {
    uint8_t mempool[SZ_1MiB];
};

static state memchr_testing(void *pdata)
{
    struct memchr_pdata *mdata = pdata;
    ktime_t start = timekeeping_get_time();
    unsigned int count = 0;
    char sbuff[32];

    do {
        if (memchr(mdata->mempool, 0xff, SZ_1MiB))
            return -EFAULT;
        count++;
    } while (ktime_after(
        start, ktime_sub_ms
        (timekeeping_get_time(), 1000)
    ));

    gsize(sbuff, count * SZ_1MiB);
    printk("memchr bandwidth: %s/s\n", sbuff);
    return -ENOERR;
}

static void *memchr_prepare(int argc, char *argv[])
{
    struct memchr_pdata *pdata;

    pdata = vmalloc(sizeof(*pdata));
    if (!pdata)
        return ERR_PTR(-ENOMEM);

    memset(pdata->mempool, 0, SZ_1MiB);
    return pdata;
}

static void memchr_release(void *pdata)
{
    vfree(pdata);
}

static struct selftest_command memchr_command = {
    .group = "string",
    .name = "memchr",
    .desc = "benchmark memchr",
    .testing = memchr_testing,
    .prepare = memchr_prepare,
    .release = memchr_release,
};

static state selftest_memchr_init(void)
{
    return selftest_register(&memchr_command);
}
kshell_initcall(selftest_memchr_init);
