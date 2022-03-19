/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <panic.h>

static void usage(void)
{
    printk("usage: panic \"message\"\n");
}

static state panic_main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        return -EINVAL;
    }
    panic("%s", argv[1]);
}

static struct kshell_command panic_cmd = {
    .name = "panic",
    .desc = "panic trigger",
    .exec = panic_main,
};

static state panic_init(void)
{
    return kshell_register(&panic_cmd);
}
kshell_initcall(panic_init);
