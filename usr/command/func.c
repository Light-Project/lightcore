/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <export.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: func [option] @name {body}\n");
    kshell_printf(ctx, "\t-d  <name> delete function\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state func_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    state retval;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'd':
                if ((++count) >= argc)
                    goto usage;
                retval = kshell_func_delete(ctx, argv[count]);
                if (!retval)
                    goto usage;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    if (*argv[count] == '@') {
        if (argc != count + 2)
            goto usage;
        return kshell_func_define(ctx, argv[count] + 1, argv[count + 1], true);
    }

    return kshell_func_call(ctx, argc - count, &argv[count]);

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command func_cmd = {
    .name = "func",
    .desc = "declare execution function",
    .exec = func_main,
};

static state func_init(void)
{
    return kshell_register(&func_cmd);
}
kshell_initcall(func_init);
