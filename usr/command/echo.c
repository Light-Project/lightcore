/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <printk.h>

static void usage(void)
{
    printk("usage: echo [option]...                 \n");
    printk("  -n  not output the trailing newline   \n");
    printk("  -h  show this help                    \n");
}

static state echo_main(int argc, char *argv[])
{
    char *para;
    bool nflag = false;

    while (--argc) {
        para = *++argv;
        if (para[0] == '-' && para[1]) {
            switch (*++para) {
                case 'n':
                    nflag = true;
                    break;
                case 'h':
                    usage();
                    return -ENOERR;
            }
            continue;
        }
        break;
    }

    while (argc--) {
        printk("%s", *argv++);
        if (*argv)
            printk(" ");
    }

    if (!nflag)
        printk("\n");

    return -ENOERR;
}

static struct kshell_command echo_cmd = {
    .name = "echo",
    .desc = "echo the string to console",
    .exec = echo_main,
};

static state echo_init(void)
{
    return kshell_register(&echo_cmd);
}
kshell_initcall(echo_init);
