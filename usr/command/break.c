/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME break
# define PROGRAM_DESC "break one loop"
# define PROGRAM_FUNC -EFAULT
#else
# define PROGRAM_DESC "continue one loop"
# define PROGRAM_FUNC -EAGAIN
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: "__stringify(PROGRAM_NAME)
                  " [ignored command line arguments]\n");
}

static state break_main(struct kshell_context *ctx, int argc, char *argv[])
{
    if (argc != 1) {
        usage(ctx);
        return -EINVAL;
    }

    ctx->breakdown = true;
    return PROGRAM_FUNC;
}

static struct kshell_command break_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = break_main,
};

static state break_init(void)
{
    return kshell_register(&break_cmd);
}
kshell_initcall(break_init);
