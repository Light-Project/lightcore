/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <string.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: try {commands} catch/finally {commands}\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state tryrun_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool catch = false, finally = false;
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

    if (argc >= count + 2) {
        if (!strcmp("catch", argv[count + 1])) {
            if (argc >= count + 4) {
                if (!strcmp("finally", argv[count + 3]))
                    finally = true;
                else
                    goto usage;
            }
            catch = true;
        } else if (!strcmp("finally", argv[count + 1]))
            finally = true;
        else
            goto usage;
    }

    ctx->tryrun = true;
    retval = kshell_system(ctx, argv[count]);
    ctx->tryrun = false;

    if (retval && catch)
        retval = kshell_system(ctx, argv[count + 2]);

    if (finally)
        retval = kshell_system(ctx, argv[count + (catch ? 4 : 2)]);

    return retval;

usage:
    usage(ctx);
    return -ENOERR;
}

static struct kshell_command tryrun_cmd = {
    .name = "try",
    .desc = "",
    .exec = tryrun_main,
};

static state tryrun_init(void)
{
    return kshell_register(&tryrun_cmd);
}
kshell_initcall(tryrun_init);
