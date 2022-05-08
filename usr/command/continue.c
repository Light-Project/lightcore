/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static void usage(void)
{
    kshell_printf("usage: continue"
                  " [ignored command line arguments]\n");
}

static state continue_main(int argc, char *argv[])
{
    if (argc != 1)
        usage();

    return -EAGAIN;
}

static struct kshell_command continue_cmd = {
    .name = "continue",
    .desc = "continue one loop",
    .exec = continue_main,
};

static state continue_init(void)
{
    return kshell_register(&continue_cmd);
}
kshell_initcall(continue_init);
