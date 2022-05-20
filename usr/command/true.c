/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

#ifndef EXIT_STATUS
# define EXIT_STATUS ENOERR
#endif

#if EXIT_STATUS == ENOERR
# define PROGRAM_NAME true
# define PROGRAM_DESC "exit code indicating success"
#else
# define PROGRAM_NAME false
# define PROGRAM_DESC "exit code indicating failure"
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: " __stringify(PROGRAM_NAME)
                  " [ignored command line arguments]\n");
}

static state true_main(struct kshell_context *ctx, int argc, char *argv[])
{
    if (argc != 1)
        usage(ctx);

    return -EXIT_STATUS;
}

static struct kshell_command true_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = true_main,
};

static state true_init(void)
{
    return kshell_register(&true_cmd);
}
kshell_initcall(true_init);
