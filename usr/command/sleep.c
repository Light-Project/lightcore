/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <delay.h>

static void usage(void)
{
	kshell_printf("usage: sleep [millisecond]\n");
}

static state delay_main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        return -EINVAL;
    }

    msleep(atoi(argv[1]));
    return -ENOERR;
}

static struct kshell_command sleep_cmd = {
    .name = "sleep",
    .desc = "sleep millisecond",
    .exec = delay_main,
};

static state sleep_init(void)
{
    return kshell_register(&sleep_cmd);
}
kshell_initcall(sleep_init);
