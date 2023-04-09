/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <kmalloc.h>
#include <cmdline.h>
#include <string.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: echo [option]...\n");
    kshell_puts(ctx, "\t-n  not output the trailing newline\n");
    kshell_puts(ctx, "\t-e  enable interpretation\n");
    kshell_puts(ctx, "\t-i  enable index information\n");
    kshell_puts(ctx, "\t-h  show this help\n");

    kshell_puts(ctx, "If -e is in effect:\n");
    kshell_puts(ctx, "\t\\\\  backslash\n");
    kshell_puts(ctx, "\t\\a  alert (BEL)\n");
    kshell_puts(ctx, "\t\\b  backspace\n");
    kshell_puts(ctx, "\t\\c  produce no further output\n");
    kshell_puts(ctx, "\t\\e  escape\n");
    kshell_puts(ctx, "\t\\f  form feed\n");
    kshell_puts(ctx, "\t\\n  new line\n");
    kshell_puts(ctx, "\t\\r  carriage return\n");
    kshell_puts(ctx, "\t\\t  horizontal tab\n");
    kshell_puts(ctx, "\t\\v  vertical tab\n");
}

static state echo_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool nflag = false, eflag = false, iflag = false;
    unsigned int index = 0;
    char *para;

    while (--argc) {
        para = *++argv;
        if (para[0] == '-' && para[1]) {
            switch (*++para) {
                case 'n':
                    nflag = true;
                    continue;

                case 'e':
                    eflag = true;
                    continue;

                case 'i':
                    iflag = true;
                    continue;

                case 'h':
                    usage(ctx);
                    return -ENOERR;
            }
        }
        break;
    }

    while (argc--) {
        if (iflag)
            kshell_printf(ctx, "%u: ", index++);
        para = *argv++;
        if (eflag)
            escape_string(para, ~0, para);
        kshell_puts(ctx, para);
        if (argc) {
            if (iflag)
                kshell_putc(ctx, '\n');
            else
                kshell_putc(ctx, ' ');
        }
    }

    if (!nflag)
        kshell_putc(ctx, '\n');

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
