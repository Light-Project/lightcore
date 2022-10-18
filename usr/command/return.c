/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <kernel.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: return [retval]\n");
}

static state return_main(struct kshell_context *ctx, int argc, char *argv[])
{
    if (argc == 1) {
        ctx->breakfunc = true;
        return -ENOERR;
    } else if (argc == 2) {
        ctx->breakfunc = true;
        return atoi(argv[1]);
    }

    usage(ctx);
    return -EINVAL;
}

static struct kshell_command return_cmd = {
    .name = "return",
    .desc = "return form function",
    .exec = return_main,
};

static state return_init(void)
{
    return kshell_register(&return_cmd);
}
kshell_initcall(return_init);
