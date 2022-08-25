/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <ilist.h>
#include <selftest.h>

#define TEST_LOOP 10

struct ilist_test_node {
    struct ilist_node ilist;
    unsigned long num;
};

struct ilist_test_pdata {
    struct ilist_test_node nodes[TEST_LOOP];
};

#define ilist_to_test(ptr) \
    ilist_entry(ptr, struct ilist_test_node, ilist)

static long ilist_test_cmp(struct ilist_node *nodea, struct ilist_node *nodeb, const void *pdata)
{
    struct ilist_test_node *inodea = ilist_to_test(nodea);
    struct ilist_test_node *inodeb = ilist_to_test(nodeb);
    if (inodea->num == inodeb->num) return 0;
    return inodea->num < inodeb->num ? -1 : 1;
}

static state ilist_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct ilist_test_pdata *idata = pdata;
    unsigned int count, count2;

    ILIST_HEAD(test_head);

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "ilist 'ilist_add' test%u single\n", count);
        idata->nodes[count].num = count;
        ilist_node_init(&idata->nodes[count].ilist);
        ilist_add(&test_head, &idata->nodes[count].ilist, ilist_test_cmp, NULL);
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &idata->nodes[count].ilist);

    for (count = 0; count < TEST_LOOP / 2; ++count) {
        for (count2 = 0; count2 < 2; ++count2) {
            kshell_printf(ctx, "ilist 'ilist_add' test%u multi%u\n", count * 2 + count2, count2);
            idata->nodes[count].num = count;
            ilist_node_init(&idata->nodes[count * 2 + count2].ilist);
            ilist_add(&test_head, &idata->nodes[count * 2 + count2].ilist, ilist_test_cmp, NULL);
        }
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &idata->nodes[count].ilist);

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
