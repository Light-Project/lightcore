/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <delay.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME sleep
# define PROGRAM_FUNC msleep
# define PROGRAM_DESC "sleep millisecond"
#else
# define PROGRAM_FUNC mdelay
# define PROGRAM_DESC "delay millisecond"
#endif

static void usage(struct kshell_context *ctx)
{
	kshell_printf(ctx, "usage: " __stringify(PROGRAM_NAME) " [millisecond]\n");
}

static state sleep_main(struct kshell_context *ctx, int argc, char *argv[])
{
    if (argc != 2) {
        usage(ctx);
        return -EINVAL;
    }

    PROGRAM_FUNC(atoi(argv[1]));
    return -ENOERR;
}

static struct kshell_command sleep_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = sleep_main,
};

static state sleep_init(void)
{
    return kshell_register(&sleep_cmd);
}
kshell_initcall(sleep_init);
