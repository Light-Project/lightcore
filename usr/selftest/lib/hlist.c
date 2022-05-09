/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <hlist.h>
#include <selftest.h>

#define TEST_LOOP 10

struct hlist_test_node {
    struct hlist_node list;
    unsigned long num;
};

struct hlist_test_pdata {
    struct hlist_test_node nodes[TEST_LOOP];
};

#define hlist_to_test(ptr) \
    hlist_entry(ptr, struct hlist_test_node, list)

static state hlist_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct hlist_test_pdata *hdata = pdata;
    struct hlist_test_node *node, *nnode, *tnode;
    struct hlist_node *list, *nlist, *tlist;
    unsigned int count;

    HLIST_HEAD(test_head);

    for (count = 0; count < ARRAY_SIZE(hdata->nodes); ++count) {
        hlist_head_add(&test_head, &hdata->nodes[count].list);
    }

    hlist_for_each(list, &test_head) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    hlist_for_each_continue(list) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each_continue' test: %lu\n", node->num);
    }

    list = tlist;
    hlist_for_each_from(list) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each_from' test: %lu\n", node->num);
    }

    hlist_for_each_safe(list, nlist, &test_head) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    hlist_for_each_continue_safe(list, nlist) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each_continue_safe' test: %lu\n", node->num);
    }

    list = tlist;
    hlist_for_each_from_safe(list, nlist) {
        node = hlist_to_test(list);
        kshell_printf(ctx, "hlist 'hlist_for_each_from_safe' test: %lu\n", node->num);
    }

    hlist_for_each_entry(node, &test_head, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    hlist_for_each_entry_continue(node, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    hlist_for_each_entry_from(node, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry_from' test: %lu\n", node->num);
    }

    hlist_for_each_entry_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
        hlist_del(&node->list);
    }

    tnode = node;
    hlist_for_each_entry_continue_safe(node, nnode, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry_continue_safe' test: %lu\n", node->num);
    }

    node = tnode;
    hlist_for_each_entry_from_safe(node, nnode, list) {
        kshell_printf(ctx, "hlist 'hlist_for_each_entry_from_safe' test: %lu\n", node->num);
        hlist_del(&node->list);
    }

    return -ENOERR;
}

static void *hlist_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct hlist_test_pdata *hdata;
    unsigned int count;

    hdata = kmalloc(sizeof(struct hlist_test_pdata), GFP_KERNEL);
    if (!hdata)
        return NULL;

    for (count = 0; count < ARRAY_SIZE(hdata->nodes); ++count)
        hdata->nodes[count].num = TEST_LOOP - count - 1;

    return hdata;
}

static void hlist_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command hlist_test_command = {
    .group = "lib",
    .name = "hlist",
    .desc = "hlist list unit test",
    .testing = hlist_test_testing,
    .prepare = hlist_test_prepare,
    .release = hlist_test_release,
};

static state selftest_hlist_init(void)
{
    return selftest_register(&hlist_test_command);
}
kshell_initcall(selftest_hlist_init);
