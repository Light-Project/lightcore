/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static state clear_main(struct kshell_context *ctx, int argc, char *argv[])
{
    kshell_puts(ctx, "\e[2J");
    return -ENOERR;
}

static struct kshell_command clear_cmd = {
    .name = "clear",
    .desc = "clear console screen",
    .exec = clear_main,
};

static state clear_init(void)
{
    return kshell_register(&clear_cmd);
}
kshell_initcall(clear_init);
