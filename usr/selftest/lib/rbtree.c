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

#define rbnode_to_test_safe(ptr) \
    rb_entry_safe(ptr, struct rbtree_test_node, node)

static long rbtest_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct rbtree_test_node *nodea = rbnode_to_test(rba);
    struct rbtree_test_node *nodeb = rbnode_to_test(rbb);
    return nodea->num < nodeb->num ? -1 : 1;
}

static long rbtest_rb_find(const struct rb_node *rb, const void *key)
{
    struct rbtree_test_node *node = rbnode_to_test(rb);
    if (node->num == (unsigned long)key) return 0;
    return node->num < (unsigned long)key ? -1 : 1;
}

static state rbtree_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct rbtree_test_pdata *sdata = pdata;
    struct rbtree_test_node *node, *nnode, *tnode;
    struct rb_node *rbnode, *nrbnode, *trbnode;
    unsigned long count;

    RB_ROOT_CACHED(test_root);

    for (count = 0; count < TEST_LOOP; ++count)
        rb_cached_insert(&test_root, &sdata->nodes[count].node, rbtest_rb_cmp);

    for (count = 0; count < TEST_LOOP; ++count) {
        rbnode = rb_find(&test_root.root, (void *)count, rbtest_rb_find);
        if (!(node = rbnode_to_test_safe(rbnode)))
            return -EFAULT;
        kshell_printf(ctx, "rbtree 'rb_cached' test: %lu\n", node->num);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        rbnode = rb_cached_find(&test_root, (void *)count, rbtest_rb_find);
        if (!(node = rbnode_to_test_safe(rbnode)))
            return -EFAULT;
        kshell_printf(ctx, "rbtree 'rb_cached_find' test: %lu\n", node->num);
    }

    count = 0;
    rb_for_each(rbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_for_each_continue(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_for_each_from(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_for_each_reverse(rbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each_reverse' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_for_each_reverse_continue(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each_reverse_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_for_each_reverse_from(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_for_each_reverse_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_post_for_each(rbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_post_for_each_continue(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_post_for_each_from(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_post_for_each_safe(rbnode, nrbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each_safe' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_post_for_each_safe_continue(rbnode, nrbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each_safe_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_post_for_each_safe_from(rbnode, nrbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each_safe_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_for_each_entry(node, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_for_each_entry_continue(node, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    rb_for_each_entry_from(node, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_for_each_entry_reverse(node, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry_reverse' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_for_each_entry_reverse_continue(node, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry_reverse_continue' test: %lu\n", node->num);
    }

    node = tnode;
    rb_for_each_entry_reverse_from(node, node) {
        kshell_printf(ctx, "rbtree 'rb_for_each_entry_reverse_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_post_for_each_entry(node, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    count = 0;
    rb_pre_for_each(rbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_pre_for_each' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_pre_for_each_continue(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_pre_for_each_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_pre_for_each_from(rbnode) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_pre_for_each_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_pre_for_each_entry(node, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_pre_for_each_entry' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_pre_for_each_entry_continue(node, node) {
        kshell_printf(ctx, "rbtree 'rb_pre_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    rb_pre_for_each_entry_from(node, node) {
        kshell_printf(ctx, "rbtree 'rb_pre_for_each_entry_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_level_for_each(rbnode, &count, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_level_for_each' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    trbnode = rbnode;
    rb_level_for_each_continue(rbnode, &count, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_level_for_each_continue' test: %lu\n", node->num);
    }

    rbnode = trbnode;
    rb_level_for_each_from(rbnode, &count, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_level_for_each_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_level_for_each_entry(node, &count, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_level_for_each_entry' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_level_for_each_entry_from(node, &count, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_level_for_each_entry_from' test: %lu\n", node->num);
    }

    node = tnode;
    rb_level_for_each_entry_continue(node, &count, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_level_for_each_entry_continue' test: %lu\n", node->num);
    }

    count = 0;
    rb_post_for_each(rbnode, &test_root.root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_post_for_each' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_post_for_each_entry_continue(node, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_continue' test: %lu\n", node->num);
    }

    node = tnode;
    rb_post_for_each_entry_from(node, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_from' test: %lu\n", node->num);
    }

    count = 0;
    rb_post_for_each_entry_safe(node, nnode, &test_root.root, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_safe' test: %lu\n", node->num);
        if (count++ == TEST_LOOP / 2)
            break;
    }

    tnode = node;
    rb_post_for_each_entry_continue_safe(node, nnode, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_continue_safe' test: %lu\n", node->num);
    }

    node = tnode;
    rb_post_for_each_entry_from_safe(node, nnode, node) {
        kshell_printf(ctx, "rbtree 'rb_post_for_each_entry_from_safe' test: %lu\n", node->num);
    }

    rb_cached_for_each(rbnode, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_for_each' test: %lu\n", node->num);
    }

    rb_cached_for_each_reverse(rbnode, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_for_each_reverse' test: %lu\n", node->num);
    }

    rb_cached_for_each_entry(node, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_for_each_entry' test: %lu\n", node->num);
    }

    rb_cached_for_each_entry_reverse(node, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_for_each_entry_reverse' test: %lu\n", node->num);
    }

    rb_cached_pre_for_each(rbnode, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_pre_for_each' test: %lu\n", node->num);
    }

    rb_cached_pre_for_each_entry(node, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_pre_for_each_entry' test: %lu\n", node->num);
    }

    rb_cached_level_for_each(rbnode, &count, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_level_for_each' test: %lu\n", node->num);
    }

    rb_cached_level_for_each_entry(node, &count, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_level_for_each_entry' test: %lu\n", node->num);
    }

    rb_cached_post_for_each(rbnode, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_post_for_each' test: %lu\n", node->num);
    }

    rb_cached_post_for_each_safe(rbnode, nrbnode, &test_root) {
        node = rbnode_to_test(rbnode);
        kshell_printf(ctx, "rbtree 'rb_cached_post_for_each_safe' test: %lu\n", node->num);
    }

    rb_cached_post_for_each_entry(node, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_post_for_each_entry' test: %lu\n", node->num);
    }

    rb_cached_post_for_each_entry_safe(node, nnode, &test_root, node) {
        kshell_printf(ctx, "rbtree 'rb_cached_post_for_each_entry_safe' test: %lu\n", node->num);
        rb_cached_delete(&test_root, &node->node);
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

    for (count = 0; count < TEST_LOOP; ++count)
        rdata->nodes[count].num = TEST_LOOP - count - 1;

    return rdata;
}

static void rbtree_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command rbtree_test_command = {
    .group = "lib",
    .name = "rbtree",
    .desc = "rbtree unit test",
    .testing = rbtree_test_testing,
    .prepare = rbtree_test_prepare,
    .release = rbtree_test_release,
};

static state selftest_rbtree_init(void)
{
    return selftest_register(&rbtree_test_command);
}
kshell_initcall(selftest_rbtree_init);
