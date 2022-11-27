/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME export
# define PROGRAM_DESC "declare environment variables"
# define PROGRAM_FUNC kshell_global_put
#else
# define PROGRAM_DESC "declare local variables"
# define PROGRAM_FUNC kshell_local_put
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: "__stringify(PROGRAM_NAME)" name[=value]...\n");
}

static state export_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    state retval;

    if (argc <= 1) {
        usage(ctx);
        retval = -EINVAL;
    }

    else for (count = 1; count < argc; ++count) {
        retval = PROGRAM_FUNC(ctx, argv[count]);
        if (retval == -EINVAL)
            kshell_printf(ctx, __stringify(PROGRAM_NAME)
                          ": not a identifier: %s\n", argv[count]);
        if (retval)
            break;
    }

    return retval;
}

static struct kshell_command export_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = export_main,
};

static state export_init(void)
{
    return kshell_register(&export_cmd);
}
kshell_initcall(export_init);
