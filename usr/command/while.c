/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <string.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME while
# define PROGRAM_DESC "loop execution when condition true"
# define PROGRAM_FUNC condition
#else
# define PROGRAM_DESC "loop execution when condition false"
# define PROGRAM_FUNC !condition
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: "__stringify(PROGRAM_NAME)" [option] 'condition' {commands}\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state while_main(struct kshell_context *ctx, int argc, char *argv[])
{
    state condition, retval = -ENOERR;
    unsigned int count;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (argc < count + 2)
        goto usage;

    for (;;) {
        condition = kshell_system(ctx, argv[count + 0]);
        if (PROGRAM_FUNC)
            break;
        if (kshell_ctrlc(ctx))
            return -ECANCELED;
        retval = kshell_system(ctx, argv[count + 1]);
        if (ctx->breakdown) {
            ctx->breakloop = false;
            if (retval != -EAGAIN)
                break;
        }
    }

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command while_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = while_main,
};

static state while_init(void)
{
    return kshell_register(&while_cmd);
}
kshell_initcall(while_init);
