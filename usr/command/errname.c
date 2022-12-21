/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <errname.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: errname [option] value...\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static inline void errname_show(struct kshell_context *ctx, state value)
{
    const char *info, *name = errname(value, &info);

    if (name)
        kshell_printf(ctx, "%s (%d): %s.\n", name, value, info);
    else
        kshell_printf(ctx, "EUNKNOW (%d).\n", value);
}

static state errname_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    state value;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h':
                goto usage;

            default:
                goto finish;
        }
    }

finish:
    if (count == argc) {
        value = atoi(kshell_symbol_get(ctx, "?") ?: "0");
        errname_show(ctx, value);
    }

    else do {
        value = atoi(argv[count]);
        errname_show(ctx, value);
    } while (++count < argc);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command errname_cmd = {
    .name = "errname",
    .desc = "show errno name",
    .exec = errname_main,
};

static state errname_init(void)
{
    return kshell_register(&errname_cmd);
}
kshell_initcall(errname_init);
