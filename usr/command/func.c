/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <export.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: func [option] name {body}\n");
    kshell_puts(ctx, "\t-d  <name> delete function\n");
    kshell_puts(ctx, "\t-w  weak declarative function\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state func_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    bool weak = false;
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
                if (retval) {
                    kshell_printf(ctx, "function '%s' not found\n", argv[count]);
                    return retval;
                }
                break;

            case 'w':
                weak = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        return -ENOERR;

    retval = kshell_func_define(ctx, argv[count], argv[count + 1], !weak);
    if (retval)
        kshell_printf(ctx, "function '%s' already\n", argv[count]);

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command func_cmd = {
    .name = "func",
    .desc = "declare function",
    .exec = func_main,
};

static state func_init(void)
{
    return kshell_register(&func_cmd);
}
kshell_initcall(func_init);
