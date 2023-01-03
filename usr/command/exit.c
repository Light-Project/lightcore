/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <kernel.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: exit [retval]\n");
}

static state exit_main(struct kshell_context *ctx, int argc, char *argv[])
{
    if (argc == 1) {
        ctx->breakexit = true;
        return -ENOERR;
    } else if (argc == 2) {
        ctx->breakexit = true;
        return atoi(argv[1]);
    }

    usage(ctx);
    return -EINVAL;
}

static struct kshell_command exit_cmd = {
    .name = "exit",
    .desc = "exit the kshell interpreter",
    .exec = exit_main,
};

static state exit_init(void)
{
    return kshell_register(&exit_cmd);
}
kshell_initcall(exit_init);
