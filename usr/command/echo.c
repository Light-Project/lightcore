/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static void usage(void)
{
    kshell_printf("usage: echo [option]...\n");
    kshell_printf("  -n  not output the trailing newline\n");
    kshell_printf("  -h  show this help\n");
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
        kshell_printf("%s", *argv++);
        if (*argv)
            kshell_printf(" ");
    }

    if (!nflag)
        kshell_printf("\n");

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
