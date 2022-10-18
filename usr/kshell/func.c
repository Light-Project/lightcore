/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: func [option] name {body}\n");
    kshell_printf(ctx, "\t-f  declaration function\n");
    kshell_printf(ctx, "\t-d  delete function\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static long func_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct kshell_func *funca = func_to_kshell(rba);
    const struct kshell_func *funcb = func_to_kshell(rbb);
    return strcmp(funca->name, funcb->name);
}

static long func_rb_find(const struct rb_node *rb, const void *name)
{
    const struct kshell_func *func = func_to_kshell(rb);
    return strcmp(name, func->name);
}

static state delete_func(struct kshell_context *ctx, char *name)
{
    struct rb_node *rb;
    struct kshell_func *node;

    rb = rb_find(&ctx->func, name, func_rb_find);
    if (rb) {
        kshell_printf(ctx, "function '%s' not found to delete\n", name);
        return -ENOENT;
    }

    node = func_to_kshell(rb);
    rb_delete(&ctx->func, &node->node);
    kfree(node);

    return -ENOERR;
}

static state define_func(struct kshell_context *ctx, char *name, char *body)
{
    struct rb_node *rb, *parent, **link;
    struct kshell_func *node;
    size_t nlen, blen;

    rb = rb_find_last(&ctx->func, name, func_rb_find, &parent, &link);
    if (rb) {
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

static state call_function(struct kshell_context *ctx, char *name, int argc, char *argv[])
{
    struct rb_node *rb;
    struct kshell_func *node;
    unsigned int count;
    char varname[12];
    state retval;

    rb = rb_find(&ctx->func, name, func_rb_find);
    if (!rb) {
        kshell_printf(ctx, "function '%s' undeclared\n", name);
        return -EALREADY;
    }

    for (count = 0; count < argc; ++count) {
        snprintf(varname, 12, "arg%d", count);
        kshell_setenv(ctx, varname, argv[count], true);
    }

    node = func_to_kshell(rb);
    retval = kshell_system(ctx, node->body);
    ctx->breakfunc = false;

    while (count--) {
        snprintf(varname, 12, "arg%d", count);
        kshell_unsetenv(ctx, varname);
    }

    return retval;
}

static state func_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool fflag = false;
    bool dflag = false;
    unsigned int count;
    state retval;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'f':
                fflag = true;
                break;

            case 'd':
                dflag = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    if (dflag) for (; count < argc; ++count) {
        retval = delete_func(ctx, argv[count]);
        if (retval)
            return retval;
    }

    else if (fflag) {
        if (argc != count + 2)
            goto usage;
        return define_func(ctx, argv[count], argv[count + 1]);
    }

    return call_function(ctx, argv[count], argc - count - 1, &argv[count + 1]);

usage:
    usage(ctx);
    return -EINVAL;
}

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
    .name = "func",
    .desc = "declare execution function",
    .exec = func_main,
    .prepare = func_prepare,
    .release = func_release,
};

static state func_init(void)
{
    return kshell_register(&func_cmd);
}
kshell_initcall(func_init);
