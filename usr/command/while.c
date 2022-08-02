/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <string.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: while [option] 'condition' {commands}\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state while_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    state retval = -ENOERR;

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

    while (!kshell_system(ctx, argv[count + 0])) {
        if (kshell_ctrlc(ctx))
            return -ECANCELED;
        retval = kshell_system(ctx, argv[count + 1]);
        if (retval && retval != -EAGAIN)
            break;
        retval = -ENOERR;
    }

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command while_cmd = {
    .name = "while",
    .desc = "circulates execute commands",
    .exec = while_main,
};

static state while_init(void)
{
    return kshell_register(&while_cmd);
}
kshell_initcall(while_init);
