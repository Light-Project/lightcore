/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <rbtree.h>
#include <selftest.h>

#define TEST_LOOP 10

struct rbtree_test_node {
    struct rb_node node;
    unsigned long num;
};

struct rbtree_test_pdata {
    struct rbtree_test_node nodes[TEST_LOOP];
};

#define rbnode_to_test(ptr) \
    rb_entry(ptr, struct rbtree_test_node, node)

static long rbtest_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct rbtree_test_node *nodea = rbnode_to_test(rba);
    struct rbtree_test_node *nodeb = rbnode_to_test(rbb);
    return nodea->num - nodeb->num;
}

static state rbtree_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct rbtree_test_pdata *sdata = pdata;
    struct rbtree_test_node *node, *tnode;
    unsigned int count;

    RB_ROOT(test_root);

    for (count = 0; count < ARRAY_SIZE(sdata->nodes); ++count)
        rb_insert(&test_root, &sdata->nodes[count].node, rbtest_rb_cmp);

    rb_for_each_entry(node, &test_root, node)
        kshell_printf(ctx, "rbtree 'rb_for_each_entry' test: %lu\n", node->num);

    rb_post_for_each_entry_safe(node, tnode, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_safe' test: %lu\n", node->num);
        rb_delete(&test_root, &node->node);
    }

    return -ENOERR;
}

static void *rbtree_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct rbtree_test_pdata *rdata;
    unsigned int count;

    rdata = kmalloc(sizeof(struct rbtree_test_pdata), GFP_KERNEL);
    if (!rdata)
        return NULL;

    for (count = 0; count < ARRAY_SIZE(rdata->nodes); ++count)
        rdata->nodes[count].num = TEST_LOOP - count - 1;

    return rdata;
}

static void rbtree_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command hlist_test_command = {
    .group = "lib",
    .name = "rbtree",
    .desc = "rbtree list unit test",
    .testing = rbtree_test_testing,
    .prepare = rbtree_test_prepare,
    .release = rbtree_test_release,
};

static state selftest_hlist_init(void)
{
    return selftest_register(&hlist_test_command);
}
kshell_initcall(selftest_hlist_init);
