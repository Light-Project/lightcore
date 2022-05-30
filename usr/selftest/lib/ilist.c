/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <ilist.h>
#include <selftest.h>

#define TEST_LOOP 10

struct ilist_test_pdata {
    struct ilist_node nodes[TEST_LOOP];
};

static state ilist_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct ilist_test_pdata *ldata = pdata;
    unsigned int count, count2;

    ILIST_HEAD(test_head);

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "ilist 'ilist_add' test%u single\n", count);
        ilist_node_init(&ldata->nodes[count], count);
        ilist_add(&test_head, &ldata->nodes[count]);
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &ldata->nodes[count]);

    for (count = 0; count < TEST_LOOP / 2; ++count) {
        for (count2 = 0; count2 < 2; ++count2) {
            kshell_printf(ctx, "ilist 'ilist_add' test%u multi%u\n", count * 2 + count2, count2);
            ilist_node_init(&ldata->nodes[count * 2 + count2], count2);
            ilist_add(&test_head, &ldata->nodes[count * 2 + count2]);
        }
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &ldata->nodes[count]);

    return -ENOERR;
}

static void *ilist_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return kmalloc(sizeof(struct ilist_test_pdata), GFP_KERNEL);;
}

static void ilist_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command ilist_test_command = {
    .group = "lib",
    .name = "ilist",
    .desc = "index list unit test",
    .testing = ilist_test_testing,
    .prepare = ilist_test_prepare,
    .release = ilist_test_release,
};

static state selftest_ilist_init(void)
{
    return selftest_register(&ilist_test_command);
}
kshell_initcall(selftest_ilist_init);
