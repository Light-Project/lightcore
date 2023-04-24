/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <string.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: repeat [var in] 'times' {commands}\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state repeat_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, loop, limit;
    const char *varname = NULL;
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

    if (argc >= count + 4 && !strcmp("in", argv[count + 1])) {
        varname = argv[count];
        count += 2;
    }

    limit = atoi(argv[count]);
    for (loop = 0; loop < limit; ++loop) {
        char buff[32];

        if (kshell_ctrlc(ctx))
            return -ECANCELED;

        if (varname) {
            itoad(loop, buff);
            kshell_setenv(ctx, varname, buff, true);
        }

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

static struct kshell_command repeat_cmd = {
    .name = "repeat",
    .desc = "repeat a command n times",
    .exec = repeat_main,
};

static state repeat_init(void)
{
    return kshell_register(&repeat_cmd);
}
kshell_initcall(repeat_init);
