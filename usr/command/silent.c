/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: silent [option] commands ...\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state silent_main(struct kshell_context *ctx, int argc, char *argv[])
{
    kshell_write_t write = NULL;
    unsigned int count;
    state retval;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    swap(write, ctx->write);
    retval = kshell_execv(ctx, argv[count], argc - count, &argv[count]);
    swap(write, ctx->write);

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command silent_cmd = {
    .name = "silent",
    .desc = "ignore all output of program",
    .exec = silent_main,
};

static state silent_init(void)
{
    return kshell_register(&silent_cmd);
}
kshell_initcall(silent_init);
