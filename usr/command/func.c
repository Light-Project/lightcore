/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <rbtree.h>
#include <kmalloc.h>
#include <initcall.h>

static RB_ROOT(func_root);

struct func_node {
    struct rb_node node;
    char *body;
    char name[0];
};

#define rb_to_func(io) \
    rb_entry(io, struct func_node, node)

static void usage(void)
{
    kshell_printf("usage: func [option] @name{body} @name(args)...\n");
    kshell_printf("\t-f  mandatory declaration function (default)\n");
    kshell_printf("\t-F  mandatory declaration function\n");
    kshell_printf("\t-d  delete function\n");
    kshell_printf("\t-h  display this message\n");
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

static state delete_func(char *name)
{
    struct rb_node *rb;
    struct func_node *node;

    rb = rb_find(&func_root, name, func_rb_find);
    if (rb) {
        kshell_printf("function '%s' not found to delete\n", name);
        return -ENOENT;
    }

    node = rb_to_func(rb);
    rb_delete(&func_root, &node->node);
    kfree(node);

    return -ENOERR;
}

static state define_func(char *name, char *body, bool force)
{
    struct rb_node *rb, *parent, **link;
    struct func_node *node;
    size_t nlen, blen;

    rb = rb_find_last(&func_root, name, func_rb_find, &parent, &link);
    if (rb) {
        if (force) {
            node = rb_to_func(rb);
            rb_delete(&func_root, rb);
            kfree(node);
        } else {
            kshell_printf("function '%s' already declared\n", name);
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
        rb_insert(&func_root, &node->node, func_rb_cmp);
    else
        rb_insert_node(&func_root, parent, link, &node->node);

    return -ENOERR;
}

static state call_function(char *name, char *args)
{
    struct rb_node *rb;
    struct func_node *node;
    unsigned int count;
    char *walk, *arg;
    char number[12];

    rb = rb_find(&func_root, name, func_rb_find);
    if (!rb) {
        kshell_printf("function '%s' undeclared\n", name);
        return -EALREADY;
    }

    for (arg = args, count = 0; arg; arg = walk, ++count) {
        walk = strchr(arg, ',');
        if (walk)
            *walk++ = '\0';
        itoa(count, number, 10);
        kshell_setenv(number, arg, true);
    }

    node = rb_to_func(rb);
    kshell_system(node->body);

    while (count--) {
        itoa(count, number, 10);
        kshell_unsetenv(number);
    }

    return -ENOERR;
}

static state func_main(int argc, char *argv[])
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
        retval = delete_func(argv[count]);
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
                retval = define_func(name, body + 1, fflag);
            else
                retval = call_function(name, body + 1);

            if (retval == -EINVAL)
                goto usage;

            break;
        }
    }

    return retval;

usage:
    usage();
    return retval;
}

static struct kshell_command func_cmd = {
    .name = "func",
    .desc = "func specifies address program",
    .exec = func_main,
};

static state func_init(void)
{
    return kshell_register(&func_cmd);
}
kshell_initcall(func_init);
