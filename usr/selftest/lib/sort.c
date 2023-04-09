/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <sort.h>
#include <kmalloc.h>
#include <prandom.h>
#include <selftest.h>
#include <initcall.h>

#define TEST_LOOP 100

struct sort_test_node {
    unsigned int index;
    unsigned int value;
};

static long sort_test_cmp(const void *nodea, const void *nodeb, void *pdata)
{
    const struct sort_test_node *testa = nodea;
    const struct sort_test_node *testb = nodeb;
    return testa->value > testb->value ? 1 : -1;
}

static state sort_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct sort_test_node *sdata = pdata;
    unsigned int count;

    for (count = 0; count < TEST_LOOP; ++count) {
        sdata[count].index = count;
        sdata[count].value = prandom_value();
        kshell_printf(ctx, "qsort generating: %02d = %u\n",
            count, sdata[count].value);
    }

    qsort(sdata, TEST_LOOP, sizeof(*sdata), sort_test_cmp, NULL);
    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "sort qsort%02u: %02u = %u\n",
            count, sdata[count].index, sdata[count].value);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        sdata[count].index = count;
        sdata[count].value = prandom_value();
        kshell_printf(ctx, "bsort generating: %02d = %u\n",
            count, sdata[count].value);
    }

    bsort(sdata, TEST_LOOP, sizeof(*sdata), sort_test_cmp, NULL);
    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "sort bsort%02u: %02u = %u\n",
            count, sdata[count].index, sdata[count].value);
    }

    return -ENOERR;
}

static void *sort_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return kmalloc(sizeof(struct sort_test_node) * TEST_LOOP, GFP_KERNEL);
}

static void sort_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command sort_test_command = {
    .group = "lib",
    .name = "sort",
    .desc = "sort unit test",
    .testing = sort_test_testing,
    .prepare = sort_test_prepare,
    .release = sort_test_release,
};

static state selftest_sort_init(void)
{
    return selftest_register(&sort_test_command);
}
kshell_initcall(selftest_sort_init);
