/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: select [option] varname in args {commands}\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

#define CHECK_PARAM {                       \
    if (!isdigit(*str) && *str != '-') {    \
        textmode = true;                    \
        break;                              \
    }                                       \
}

static state select_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, walk, index;
    char *varname;
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

    if (argc < count + 4)
        goto usage;

    if (strcmp(argv[count + 1], "in"))
        goto usage;

    varname = argv[count + 0];
    count += 2;

    for (;;) {
        for (walk = count; walk < argc - 1; ++walk) {
            kshell_printf(ctx, "%d) %s\n", walk -
                          count + 1, argv[walk]);
        }

        kshell_scanf(ctx, "%u", &index);
        if (index >= argc - count)
            continue;

        if (index == 0) {
            retval = -ECANCELED;
            break;
        }

        kshell_setenv(ctx, varname, argv[count + index - 1], true);
        retval = kshell_system(ctx, argv[argc - 1]);
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

static struct kshell_command select_cmd = {
    .name = "select",
    .desc = "menu creation",
    .exec = select_main,
};

static state select_init(void)
{
    return kshell_register(&select_cmd);
}
kshell_initcall(select_init);
