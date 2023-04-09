/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <crash.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: panic [option] \"message\"\n");
    kshell_puts(ctx, "\t-w  trigger a warn\n");
    kshell_puts(ctx, "\t-b  trigger a bug\n");
    kshell_puts(ctx, "\t-p  trigger a panic\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state panic_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'w':
                WARN();
                return -ENOERR;

            case 'b':
                BUG();

            case 'p':
                goto finish;

            case 'h': default:
                goto usage;
        }
    }

finish:
    if (count >= argc)
        goto usage;

    panic("%s", argv[count]);
    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command panic_cmd = {
    .name = "panic",
    .desc = "panic trigger",
    .exec = panic_main,
};

static state panic_init(void)
{
    return kshell_register(&panic_cmd);
}
kshell_initcall(panic_init);
