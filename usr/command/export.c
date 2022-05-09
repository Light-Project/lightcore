/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: export [name=value] ...\n");
}

static state export_main(struct kshell_context *ctx, int argc, char *argv[])
{
    int count;
    state ret;

    for (count = 1; count < argc; ++count) {
        if (!strchr(argv[count], '=')) {
            usage(ctx);
            return -EINVAL;
        }
        if ((ret = kshell_putenv(ctx, argv[count])))
            return ret;
    }

    return -ENOERR;
}

static struct kshell_command export_cmd = {
    .name = "export",
    .desc = "setting environment variables",
    .exec = export_main,
};

static state export_init(void)
{
    return kshell_register(&export_cmd);
}
kshell_initcall(export_init);
