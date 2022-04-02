/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <slist.h>
#include <selftest.h>

#define TEST_LOOP 10

struct slist_test_node {
    struct slist_head list;
    unsigned long num;
};

struct slist_test_pdata {
    struct slist_test_node nodes[TEST_LOOP];
};

#define slist_to_test(ptr) \
    list_entry(ptr, struct slist_test_node, list)

static state slist_test_testing(void *pdata)
{
    struct slist_test_pdata *sdata = pdata;
    struct slist_test_node *node, *nnode, *tnode;
    struct slist_head *list, *nlist, *tlist;
    unsigned int count;

    SLIST_HEAD(test_head);

    for (count = 0; count < ARRAY_SIZE(sdata->nodes); ++count) {
        slist_add(&test_head, &sdata->nodes[count].list);
    }

    slist_for_each(list, &test_head) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    slist_for_each_continue(list) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each_continue' test: %lu\n", node->num);
    }

    list = tlist;
    slist_for_each_from(list) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each_from' test: %lu\n", node->num);
    }

    slist_for_each_safe(list, nlist, &test_head) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tlist = list;
    slist_for_each_continue_safe(list, nlist) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each_continue_safe' test: %lu\n", node->num);
    }

    list = tlist;
    slist_for_each_from_safe(list, nlist) {
        node = slist_to_test(list);
        kshell_printf("slist 'slist_for_each_from_safe' test: %lu\n", node->num);
    }

    slist_for_each_entry(node, &test_head, list) {
        kshell_printf("slist 'slist_for_each_entry' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    slist_for_each_entry_continue(node, list) {
        kshell_printf("slist 'slist_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    slist_for_each_entry_from(node, list) {
        kshell_printf("slist 'slist_for_each_entry_from' test: %lu\n", node->num);
    }

    slist_for_each_entry_safe(node, nnode, &test_head, list) {
        kshell_printf("slist 'slist_for_each_entry_safe' test: %lu\n", node->num);
        if (node->num == TEST_LOOP / 2)
            break;
        slist_del(&test_head, &node->list);
    }

    tnode = node;
    slist_for_each_entry_continue_safe(node, nnode, list) {
        kshell_printf("slist 'slist_for_each_entry_continue_safe' test: %lu\n", node->num);
    }

    node = tnode;
    slist_for_each_entry_from_safe(node, nnode, list) {
        kshell_printf("slist 'slist_for_each_entry_from_safe' test: %lu\n", node->num);
        slist_del(&test_head, &node->list);
    }

    return -ENOERR;
}

static void *slist_test_prepare(int argc, char *argv[])
{
    struct slist_test_pdata *sdata;
    unsigned int count;

    sdata = kmalloc(sizeof(struct slist_test_pdata), GFP_KERNEL);
    if (!sdata)
        return NULL;

    for (count = 0; count < ARRAY_SIZE(sdata->nodes); ++count)
        sdata->nodes[count].num = TEST_LOOP - count - 1;

    return sdata;
}

static void slist_test_release(void *pdata)
{
    kfree(pdata);
}

static struct selftest_command hlist_test_command = {
    .group = "lib",
    .name = "slist",
    .desc = "slist list unit test",
    .testing = slist_test_testing,
    .prepare = slist_test_prepare,
    .release = slist_test_release,
};

static state selftest_hlist_init(void)
{
    return selftest_register(&hlist_test_command);
}
kshell_initcall(selftest_hlist_init);
