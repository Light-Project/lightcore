/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <uuid.h>
#include <string.h>
#include <prandom.h>
#include <btree.h>
#include <selftest.h>

#define TEST_LOOP 100

struct btree_test_node {
    struct list_head list;
    union {
        uintptr_t key;
        char uuid[UUID_STRING_LEN + 1];
    };
};

static state btree_test_clash(struct btree_root *root, void *value, void *clash)
{
    struct btree_test_node *vnode = value;
    struct btree_test_node *cnode = clash;
    list_add(&vnode->list, &cnode->list);
    return -ENOERR;
}

static void *btree_test_remove(struct btree_root *root, void *value)
{
    struct btree_test_node *vnode = value;
    struct btree_test_node *remove;

    if (list_check_empty(&vnode->list))
        return NULL;

    remove = list_first_entry(&vnode->list, struct btree_test_node, list);
    list_del(&remove->list);

    return remove;
}

static long btree_test_strfind(struct btree_root *root, uintptr_t *node, uintptr_t *key)
{
    const char *nstring = (void *)*node ?: "";
    const char *kstring = (void *)*key ?: "";
    return strcmp(nstring, kstring);
}

static state btree_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct btree_test_node *lookup, *nodes = pdata;
    unsigned int count;
    uintptr_t insert;
    void *value;
    state retval;

    BTREE_ROOT(root32, &btree_layout32,
        btree_alloc, btree_free, btree_keyfind,
        btree_test_clash, btree_test_remove, NULL
    );

    for (count = 0; count < TEST_LOOP; ++count) {
        list_head_init(&nodes[count].list);
        nodes[count].key = (uintptr_t)prandom_value();
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        retval = btree_insert(&root32, &nodes[count].key, &nodes[count], GFP_KERNEL);
        kshell_printf(ctx, "btree random insert test%d: %#010lx ret %d\n", count,
                      nodes[count].key, retval);
        if (retval)
            return retval;
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        lookup = btree_lookup(&root32, &nodes[count].key);
        kshell_printf(ctx, "btree random lookup test%d: ", count);
        if (!lookup || (lookup != &nodes[count] && list_check_empty(&lookup->list))) {
            kshell_printf(ctx, "failed\n");
            return -ENOENT;
        }
        kshell_printf(ctx, "pass\n");
    }

    btree_for_each(&root32, &insert, value) {
        kshell_printf(ctx, "btree random for each: %#010lx = %p\n",
                      insert, value);
    }

    btree_for_each_reverse(&root32, &insert, value) {
        kshell_printf(ctx, "btree random for each reverse: %#010lx = %p\n",
                      insert, value);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        lookup = btree_remove(&root32, &nodes[count].key);
        kshell_printf(ctx, "btree random remove test%d\n", count);
    }

    btree_destroy(&root32);

    BTREE_ROOT(rootstr, &btree_layout32,
        btree_alloc, btree_free, btree_test_strfind,
        btree_test_clash, btree_test_remove, NULL
    );

    for (count = 0; count < TEST_LOOP; ++count) {
        list_head_init(&nodes[count].list);
        uuid_string_generate(nodes[count].uuid);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        insert = (uintptr_t)&nodes[count].uuid;
        retval = btree_insert(&rootstr, &insert, &nodes[count], GFP_KERNEL);
        kshell_printf(ctx, "btree uuid insert test%d: %s ret %d\n", count,
                      nodes[count].uuid, retval);
        if (retval)
            return retval;
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        insert = (uintptr_t)&nodes[count].uuid;
        lookup = btree_lookup(&rootstr, &insert);
        kshell_printf(ctx, "btree uuid lookup test%d: ", count);
        if (!lookup || (lookup != &nodes[count] && list_check_empty(&lookup->list))) {
            kshell_printf(ctx, "failed\n");
            return -ENOENT;
        }
        kshell_printf(ctx, "pass\n");
    }

    btree_for_each(&rootstr, &insert, value) {
        kshell_printf(ctx, "btree uuid for each: %s = %p\n",
                      (char *)insert, value);
    }

    btree_for_each_reverse(&rootstr, &insert, value) {
        kshell_printf(ctx, "btree uuid for each reverse: %s = %p\n",
                      (char *)insert, value);
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        insert = (uintptr_t)&nodes[count].uuid;
        lookup = btree_remove(&rootstr, &insert);
        kshell_printf(ctx, "btree uuid remove test%d\n", count);
    }

    btree_destroy(&rootstr);

    return -ENOERR;
}

static void *btree_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return kmalloc(sizeof(struct btree_test_node) * TEST_LOOP, GFP_KERNEL);
}

static void btree_test_release(struct kshell_context *ctx, void *pdata)
{
    kfree(pdata);
}

static struct selftest_command btree_test_command = {
    .group = "lib",
    .name = "btree",
    .desc = "balance tree unit test",
    .testing = btree_test_testing,
    .prepare = btree_test_prepare,
    .release = btree_test_release,
};

static state selftest_btree_init(void)
{
    return selftest_register(&btree_test_command);
}
kshell_initcall(selftest_btree_init);
