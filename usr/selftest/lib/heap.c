/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <heap.h>
#include <prandom.h>
#include <selftest.h>

#define TEST_LOOP 10

struct heap_test_node {
    struct heap_node node;
    unsigned short num;
};

struct heap_test_pdata {
    struct heap_test_node nodes[TEST_LOOP];
};

#define hpnode_to_test(ptr) \
    heap_entry(ptr, struct heap_test_node, node)

static long heap_test_cmp(const struct heap_node *hpa, const struct heap_node *hpb)
{
    struct heap_test_node *nodea = hpnode_to_test(hpa);
    struct heap_test_node *nodeb = hpnode_to_test(hpb);
    return nodea->num < nodeb->num ? -1 : 1;
}

static state heap_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct heap_test_pdata *hdata = pdata;
    struct heap_test_node *node, *tnode;
    struct heap_node *hpnode, *thpnode;
    unsigned long count, index, tindex;

    HEAP_ROOT(heap_root);

    for (count = 0; count < TEST_LOOP; ++count)
        heap_insert(&heap_root, &hdata->nodes[count].node, heap_test_cmp);

    count = 0;
    heap_for_each(hpnode, &index, &heap_root) {
        node = hpnode_to_test(hpnode);
        kshell_printf(ctx, "heap 'heap_for_each' test: %u\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    thpnode = hpnode;
    tindex = index;
    heap_for_each_continue(hpnode, &index, &heap_root) {
        node = hpnode_to_test(hpnode);
        kshell_printf(ctx, "heap 'heap_for_each_continue' test: %u\n", node->num);
    }

    hpnode = thpnode;
    index = tindex;
    heap_for_each_from(hpnode, &index, &heap_root) {
        node = hpnode_to_test(hpnode);
        kshell_printf(ctx, "heap 'heap_for_each_from' test: %u\n", node->num);
    }

    count = 0;
    heap_for_each_entry(node, &index, &heap_root, node) {
        kshell_printf(ctx, "heap 'heap_for_each_entry' test: %u\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    tindex = index;
    heap_for_each_entry_continue(node, &index, &heap_root, node) {
        kshell_printf(ctx, "heap 'heap_for_each_entry_continue' test: %u\n", node->num);
    }

    node = tnode;
    index = tindex;
    heap_for_each_entry_from(node, &count, &heap_root, node) {
        kshell_printf(ctx, "heap 'heap_for_each_entry_from' test: %u\n", node->num);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        node = hpnode_to_test(heap_root.node);
        kshell_printf(ctx, "heap 'heap_delete' test: %u\n", node->num);
        heap_delete(&heap_root, &node->node, heap_test_cmp);
    }

    return -ENOERR;
}

static void *heap_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct heap_test_pdata *rdata;
    unsigned int count;

    rdata = kmalloc(sizeof(struct heap_test_pdata), GFP_KERNEL);
    if (!rdata)
        return NULL;

    for (count = 0; count < TEST_LOOP; ++count)
        rdata->nodes[count].num = prandom_value();

    return rdata;
}

static void heap_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command hlist_test_command = {
    .group = "lib",
    .name = "heap",
    .desc = "heap list unit test",
    .testing = heap_test_testing,
    .prepare = heap_test_prepare,
    .release = heap_test_release,
};

static state selftest_hlist_init(void)
{
    return selftest_register(&hlist_test_command);
}
kshell_initcall(selftest_hlist_init);
