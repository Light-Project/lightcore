/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <prandom.h>
#include <segtree.h>
#include <selftest.h>

#define TEST_LOOP 10

struct segtree_test_node {
    struct segtree_node segnode;
    unsigned int num;
};

struct segtree_test_pdata {
    struct segtree_test_node nodes[TEST_LOOP];
    unsigned int queries[TEST_LOOP];
};

#define segtree_to_test(ptr) \
    segtree_entry(ptr, struct segtree_test_node, segnode)

static state segtree_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct segtree_test_pdata *sdata = pdata;
    struct segtree_test_node *node, *tnode;
    struct segtree_node *snode, *tsnode;
    unsigned int count;

    RB_ROOT_CACHED(segtree_root);

    for (count = 0; count < TEST_LOOP; ++count)
        segtree_insert(&segtree_root, &sdata->nodes[count].segnode);

    for (count = 0; count < TEST_LOOP; ++count) {
        segtree_for_each_entry(node, 0, sdata->queries[count], &segtree_root, segnode) {
            kshell_printf(ctx, "srgtree search %u: %lu - %lu\n", sdata->queries[count],
                          node->segnode.start, node->segnode.end);
        }
    }

    count = 0;
    segtree_for_each(snode, 0, ~0UL, &segtree_root) {
        node = segtree_to_test(snode);
        kshell_printf(ctx, "srgtree 'segtree_for_each' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tsnode = snode;
    segtree_for_each_form(snode, 0, ~0UL) {
        node = segtree_to_test(snode);
        kshell_printf(ctx, "srgtree 'segtree_for_each_form' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
    }

    snode = tsnode;
    segtree_for_each_continue(snode, 0, ~0UL) {
        node = segtree_to_test(snode);
        kshell_printf(ctx, "srgtree 'segtree_for_each_continue' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
    }

    count = 0;
    segtree_for_each_entry(node, 0, ~0UL, &segtree_root, segnode) {
        kshell_printf(ctx, "srgtree 'segtree_for_each_entry' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    segtree_for_each_entry_from(node, 0, ~0UL, segnode) {
        kshell_printf(ctx, "srgtree 'segtree_for_each_entry_from' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
    }

    node = tnode;
    segtree_for_each_entry_continue(node, 0, ~0UL, segnode) {
        kshell_printf(ctx, "srgtree 'segtree_for_each_entry_continue' test: %lu - %lu\n",
                      node->segnode.start, node->segnode.end);
    }

    for (count = 0; count < TEST_LOOP; ++count)
        segtree_delete(&segtree_root, &sdata->nodes[count].segnode);

    return -ENOERR;
}

static void *segtree_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct segtree_test_pdata *sdata;
    unsigned long random;
    unsigned int count;

    sdata = kmalloc(sizeof(struct segtree_test_pdata), GFP_KERNEL);
    if (!sdata)
        return NULL;

    for (count = 0; count < ARRAY_SIZE(sdata->nodes); ++count) {
        sdata->queries[count] = prandom_value();
        sdata->nodes[count].segnode.end = (random = prandom_value());
        sdata->nodes[count].segnode.start = prandom_value() % random;
    }

    return sdata;
}

static void segtree_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command segtree_test_command = {
    .group = "lib",
    .name = "segtree",
    .desc = "segtree unit test",
    .testing = segtree_test_testing,
    .prepare = segtree_test_prepare,
    .release = segtree_test_release,
};

static state selftest_segtree_init(void)
{
    return selftest_register(&segtree_test_command);
}
kshell_initcall(selftest_segtree_init);
