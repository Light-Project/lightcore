/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <prandom.h>
#include <timerqueue.h>
#include <selftest.h>

#define TEST_LOOP 100

struct skiplist_test {
    struct tqueue_head head;
    struct tqueue_node nodes[TEST_LOOP];
};

static state timerqueue_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct skiplist_test *test = pdata;
    struct tqueue_node *node, *tnode;
    unsigned int count;

    tqueue_init(&test->head);

    for (count = 0; count < TEST_LOOP; ++count) {
        tqueue_node_init(&test->nodes[count]);
        test->nodes[count].timer = prandom_u64();
        tqueue_insert(&test->head, &test->nodes[count]);
    }

    tqueue_for_each(node, &test->head) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each' test: %llu\n", node->timer);
    }

    tnode = node;
    tqueue_for_each_continue(node) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each_continue' test: %llu\n", node->timer);
    }

    node = tnode;
    tqueue_for_each_from(node) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each_from' test: %llu\n", node->timer);
    }

    tqueue_for_each_reverse(node, &test->head) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each_reverse' test: %llu\n", node->timer);
    }

    tnode = node;
    tqueue_for_each_reverse_continue(node) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each_reverse_continue' test: %llu\n", node->timer);
    }

    node = tnode;
    tqueue_for_each_reverse_from(node) {
        kshell_printf(ctx, "rbtree 'tqueue_for_each_reverse_from' test: %llu\n", node->timer);
    }

    return -ENOERR;
}

static void *timerqueue_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return kmalloc(sizeof(struct skiplist_test), GFP_KERNEL);
}

static void timerqueue_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command timerqueue_test_command = {
    .group = "lib",
    .name = "timerqueue",
    .desc = "timerqueue unit test",
    .testing = timerqueue_test_testing,
    .prepare = timerqueue_test_prepare,
    .release = timerqueue_test_release,
};

static state selftest_timerqueue_init(void)
{
    return selftest_register(&timerqueue_test_command);
}
kshell_initcall(selftest_timerqueue_init);
