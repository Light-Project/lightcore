/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

static void usage(void)
{
    kshell_printf("usage: export [name=value] ...\n");
}

static state export_main(int argc, char *argv[])
{
    int count;
    state ret;

    for (count = 1; count < argc; ++count) {
        if (!strchr(argv[count], '=')) {
            usage();
            return -EINVAL;
        }
        if ((ret = kshell_putenv(argv[count])))
            return ret;
    }

    return -ENOERR;
}

static struct kshell_command export_cmd = {
    .name = "export",
    .desc = "export the string to console",
    .exec = export_main,
};

static state export_init(void)
{
    return kshell_register(&export_cmd);
}
kshell_initcall(export_init);
