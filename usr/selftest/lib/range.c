/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <range.h>
#include <selftest.h>

struct range_test_entry {
    unsigned long start;
    unsigned long size;
    unsigned int type;
};

struct range_test_entry range_test_table[] = {
    { 0x80000000, 0x40000000, 0 }, /* 0: base range */
    { 0x80000000, 0x20000000, 1 }, /* 1: inside front cover */
    { 0xa0000000, 0x20000000, 1 }, /* 2: inside back cover */
    { 0x70000000, 0x20000000, 2 }, /* 3: outside front cover */
    { 0xb0000000, 0x20000000, 3 }, /* 4: outside back cover */
    { 0x78000000, 0x50000000, 4 }, /* 5: completely cover */
    { 0x90000000, 0x20000000, 5 }, /* 6: middle cover */
};

static struct range_node *range_test_alloc(void *pdata)
{
    return kmalloc(sizeof(struct range_node), GFP_KERNEL);
}

static void range_test_free(struct range_node *node, void *pdata)
{
    kfree(node);
}

static void range_test_dump(struct kshell_context *ctx, struct range_head *head)
{
    struct range_node *node;
    unsigned long end;

    list_for_each_entry(node, &head->nodes, list) {
        end = node->start + node->size - 1;
        kshell_printf(ctx, "  [%#010lx - %#010lx] %d\n",
                      node->start, end, node->type);
    }
}

static state range_test_testing(struct kshell_context *ctx, void *pdata)
{
    DEFINE_RANGE(range_test, range_test_alloc, range_test_free, NULL, NULL);
    unsigned int count;
    state retval;

    for (count = 0; count < ARRAY_SIZE(range_test_table); ++count) {
        struct range_test_entry *entry = range_test_table + count;
        retval = PTR_ERR_ZERO(range_insert(&range_test, entry->type,
                 entry->start, entry->size, RANGE_DISLODGE, true));
        kshell_printf(ctx, "range dislodge test%u: %d\n", count, retval);
        if (retval) {
            range_release(&range_test);
            return -ENOMEM;
        }
        range_test_dump(ctx, &range_test);
    }
    range_release(&range_test);

    for (count = 0; count < ARRAY_SIZE(range_test_table); ++count) {
        struct range_test_entry *entry = range_test_table + count;
        retval = PTR_ERR_ZERO(range_insert(&range_test, entry->type,
                 entry->start, entry->size, RANGE_RETRACTION, true));
        kshell_printf(ctx, "range retraction test%u: %d\n", count, retval);
        if (retval && retval != -ENODATA) {
            range_release(&range_test);
            return -ENOMEM;
        }
        range_test_dump(ctx, &range_test);
    }
    range_release(&range_test);

    return -ENOERR;
}

static struct selftest_command range_test_command = {
    .group = "lib",
    .name = "range",
    .desc = "range unit test",
    .testing = range_test_testing,
};

static state selftest_range_init(void)
{
    return selftest_register(&range_test_command);
}
kshell_initcall(selftest_range_init);
