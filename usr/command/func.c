/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>

struct func_node {
    struct rb_node node;
    char *body;
    char name[0];
};

#define rb_to_func(io) \
    rb_entry(io, struct func_node, node)

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: func [option] @name{body} @name(args)...\n");
    kshell_printf(ctx, "\t-f  mandatory declaration function (default)\n");
    kshell_printf(ctx, "\t-F  mandatory declaration function\n");
    kshell_printf(ctx, "\t-d  delete function\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static long func_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct func_node *funca = rb_to_func(rba);
    const struct func_node *funcb = rb_to_func(rbb);
    return strcmp(funca->name, funcb->name);
}

static long func_rb_find(const struct rb_node *rb, const void *name)
{
    const struct func_node *func = rb_to_func(rb);
    return strcmp(name, func->name);
}

static state delete_func(struct kshell_context *ctx, char *name)
{
    struct rb_node *rb;
    struct func_node *node;

    rb = rb_find(&ctx->func, name, func_rb_find);
    if (rb) {
        kshell_printf(ctx, "function '%s' not found to delete\n", name);
        return -ENOENT;
    }

    node = rb_to_func(rb);
    rb_delete(&ctx->func, &node->node);
    kfree(node);

    return -ENOERR;
}

static state define_func(struct kshell_context *ctx, char *name, char *body, bool force)
{
    struct rb_node *rb, *parent, **link;
    struct func_node *node;
    size_t nlen, blen;

    rb = rb_find_last(&ctx->func, name, func_rb_find, &parent, &link);
    if (rb) {
        if (force) {
            node = rb_to_func(rb);
            rb_delete(&ctx->func, rb);
            kfree(node);
        } else {
            kshell_printf(ctx, "function '%s' already declared\n", name);
            return -EALREADY;
        }
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

static state call_function(struct kshell_context *ctx, char *name, char *args)
{
    struct rb_node *rb;
    struct func_node *node;
    unsigned int count;
    char *walk, *arg;
    char number[12];

    rb = rb_find(&ctx->func, name, func_rb_find);
    if (!rb) {
        kshell_printf(ctx, "function '%s' undeclared\n", name);
        return -EALREADY;
    }

    for (arg = args, count = 0; arg; arg = walk, ++count) {
        walk = strchr(arg, ',');
        if (walk)
            *walk++ = '\0';
        itoa(count, number, 10);
        kshell_setenv(ctx, number, arg, true);
    }

    node = rb_to_func(rb);
    kshell_system(ctx, node->body);

    while (count--) {
        itoa(count, number, 10);
        kshell_unsetenv(ctx, number);
    }

    return -ENOERR;
}

static state func_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    bool fflag = true;
    bool dflag = false;
    state retval = -EINVAL;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'f':
                fflag = true;
                break;

            case 'F':
                fflag = false;
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
            break;
    }

    else for (; count < argc; ++count) {
        char *body, *name = argv[count];

        if (*name++ != '@')
            goto usage;

        for (body = name; (body = strchr(name, '{')) || (body = strchr(name, '('));) {
            char *walk = body + 1;
            char brack = *body;
            unsigned int stack;

            for (stack = 1; *walk; ++walk) {
                if (*walk == brack)
                    ++stack;
                else if (*walk == (brack == '{' ? '}' : ')')) {
                    if (!--stack)
                        break;
                }
            }

            if (stack)
                goto usage;

            *body = '\0';
            *walk = '\0';

            if (brack == '{')
                retval = define_func(ctx, name, body + 1, fflag);
            else
                retval = call_function(ctx, name, body + 1);

            if (retval == -EINVAL)
                goto usage;

            break;
        }
    }

    return retval;

usage:
    usage(ctx);
    return retval;
}

static state func_prepare(struct kshell_context *ctx)
{
    ctx->func = RB_INIT;
    return -ENOERR;
}

static void func_release(struct kshell_context *ctx)
{
    struct func_node *node, *tmp;
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
