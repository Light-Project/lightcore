/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <list.h>
#include <selftest.h>

#define TEST_LOOP 10

struct list_test_node {
    struct list_head list;
    unsigned long num;
};

struct list_test_pdata {
    struct list_test_node nodes[TEST_LOOP];
};

#define list_to_test(ptr) \
    list_entry(ptr, struct list_test_node, list)

static long list_test_sort(struct list_head *nodea, struct list_head *nodeb, void *pdata)
{
    struct list_test_node *testa = list_to_test(nodea);
    struct list_test_node *testb = list_to_test(nodeb);
    return testa->num - testb->num;
}

static state list_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct list_test_pdata *ldata = pdata;
    struct list_test_node *node, *nnode, *tnode;
    struct list_head *list, *nlist, *tlist;
    unsigned int count;

    LIST_HEAD(test_head);

    for (count = 0; count < ARRAY_SIZE(ldata->nodes); ++count) {
        if (count % 1)
            list_add_prev(&test_head, &ldata->nodes[count].list);
        else
            list_add(&test_head, &ldata->nodes[count].list);
    }

    list_qsort(&test_head, list_test_sort, NULL);

    list_for_each(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    list_for_each_continue(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_continue' test: %lu\n", node->num);
    }

    list = tlist;
    list_for_each_from(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_from' test: %lu\n", node->num);
    }

    list_for_each_reverse(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    list_for_each_reverse_continue(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse_continue' test: %lu\n", node->num);
    }

    list = tlist;
    list_for_each_reverse_from(list, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse_from' test: %lu\n", node->num);
    }

    list_for_each_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    list_for_each_continue_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_continue_safe' test: %lu\n", node->num);
    }

    list = tlist;
    list_for_each_from_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_from_safe' test: %lu\n", node->num);
    }

    list_for_each_reverse_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    list_for_each_reverse_continue_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse_continue_safe' test: %lu\n", node->num);
    }

    list = tlist;
    list_for_each_reverse_from_safe(list, nlist, &test_head) {
        node = list_to_test(list);
        kshell_printf(ctx, "list 'list_for_each_reverse_from_safe' test: %lu\n", node->num);
    }

    list_for_each_entry(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    list_for_each_entry_continue(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    list_for_each_entry_from(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_from' test: %lu\n", node->num);
    }

    list_for_each_entry_reverse(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    list_for_each_entry_reverse_continue(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse_continue' test: %lu\n", node->num);
    }

    node = tnode;
    list_for_each_entry_reverse_from(node, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse_from' test: %lu\n", node->num);
    }

    list_for_each_entry_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    list_for_each_entry_continue_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_continue_safe' test: %lu\n", node->num);
    }

    node = tnode;
    list_for_each_entry_from_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_form_safe' test: %lu\n", node->num);
    }

    list_for_each_entry_reverse_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
        list_del(&node->list);
    }

    tnode = node;
    list_for_each_entry_reverse_continue_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse_continue_safe' test: %lu\n", node->num);
    }

    node = tnode;
    list_for_each_entry_reverse_from_safe(node, nnode, &test_head, list) {
        kshell_printf(ctx, "list 'list_for_each_entry_reverse_form_safe' test: %lu\n", node->num);
        list_del(&node->list);
    }

    return -ENOERR;
}

static void *list_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct list_test_pdata *ldata;
    unsigned int count;

    ldata = kmalloc(sizeof(struct list_test_pdata), GFP_KERNEL);
    if (!ldata)
        return NULL;

    for (count = 0; count < ARRAY_SIZE(ldata->nodes); ++count)
        ldata->nodes[count].num = count;

    return ldata;
}

static void list_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command list_test_command = {
    .group = "lib",
    .name = "list",
    .desc = "list unit test",
    .testing = list_test_testing,
    .prepare = list_test_prepare,
    .release = list_test_release,
};

static state selftest_list_init(void)
{
    return selftest_register(&list_test_command);
}
kshell_initcall(selftest_list_init);
