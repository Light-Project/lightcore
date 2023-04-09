/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <export.h>

static long func_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct kshell_func *funca = func_to_kshell(rba);
    const struct kshell_func *funcb = func_to_kshell(rbb);
    return strcmp(funca->name, funcb->name);
}

static long func_rb_find(const struct rb_node *rb, const void *name)
{
    const struct kshell_func *func = func_to_kshell(rb);
    return strcmp(func->name, name);
}

bool kshell_func_call(struct kshell_context *ctx, int argc, char *argv[], state *retptr)
{
    struct rb_node *rb;
    struct kshell_func *node;
    unsigned int count;
    char number[13];
    state retval;

    rb = rb_find(&ctx->func, argv[0], func_rb_find);
    if (!rb)
        return false;

    kshell_local_push(ctx);
    kshell_symbol_push(ctx);

    intoa(argc, number, 10, 12);
    kshell_symbol_set(ctx, "#", number, false);
    kshell_symbol_set(ctx, "0", argv[0], false);

    for (count = 1; count < argc; ++count) {
        intoa(count, number, 10, 12);
        kshell_symbol_set(ctx, number, argv[count], false);
    }

    node = func_to_kshell(rb);
    retval = kshell_system(ctx, node->body);
    ctx->breakfunc = false;

    kshell_symbol_pop(ctx);
    kshell_local_pop(ctx);

    if (retptr)
        *retptr = retval;

    return true;
}
EXPORT_SYMBOL(kshell_func_call);

state kshell_func_define(struct kshell_context *ctx, const char *name, const char *body, bool overwrite)
{
    struct rb_node *rb, *parent, **link;
    struct kshell_func *node;
    size_t nlen, blen;

    rb = rb_find_last(&ctx->func, name, func_rb_find, &parent, &link);
    if (rb) {
        if (!overwrite)
            return -EALREADY;
        node = func_to_kshell(rb);
        rb_delete(&ctx->func, rb);
        kfree(node);
    }

    nlen = strlen(name) + 1;
    blen = strlen(body) + 1;

    node = kmalloc(sizeof(*node) + nlen + blen, GFP_KERNEL);
    if (!node)
        return -ENOMEM;

    node->body = node->name + nlen;
    strcpy(node->body, body);
    strcpy(node->name, name);

    if (rb)
        rb_insert(&ctx->func, &node->node, func_rb_cmp);
    else
        rb_insert_node(&ctx->func, parent, link, &node->node);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_func_define);

state kshell_func_delete(struct kshell_context *ctx, const char *name)
{
    struct rb_node *rb;
    struct kshell_func *node;

    rb = rb_find(&ctx->func, name, func_rb_find);
    if (unlikely(!rb))
        return -ENOENT;

    node = func_to_kshell(rb);
    rb_delete(&ctx->func, &node->node);
    kfree(node);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_func_delete);

static state func_prepare(struct kshell_context *ctx, struct kshell_context *old)
{
    ctx->func = RB_INIT;
    return -ENOERR;
}

static void func_release(struct kshell_context *ctx)
{
    struct kshell_func *node, *tmp;
    rb_post_for_each_entry_safe(node, tmp, &ctx->func, node)
        kfree(node);
}

static struct kshell_command func_cmd = {
    .prepare = func_prepare,
    .release = func_release,
};

static state func_init(void)
{
    return kshell_register(&func_cmd);
}
kshell_initcall(func_init);
