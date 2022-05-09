/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <ctype.h>
#include <string.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: echo [option]...\n");
    kshell_printf(ctx, "\t-n  not output the trailing newline\n");
    kshell_printf(ctx, "\t-N  output the trailing newline (default)\n");
    kshell_printf(ctx, "\t-e  enable interpretation\n");
    kshell_printf(ctx, "\t-E  disable interpretation (default)\n");
    kshell_printf(ctx, "\t-i  enable index information\n");
    kshell_printf(ctx, "\t-I  disable index information (default)\n");
    kshell_printf(ctx, "\t-h  show this help\n");

    kshell_printf(ctx, "If -e is in effect:\n");
    kshell_printf(ctx, "\t\\\\  backslash\n");
    kshell_printf(ctx, "\t\\a  alert (BEL)\n");
    kshell_printf(ctx, "\t\\b  backspace\n");
    kshell_printf(ctx, "\t\\c  produce no further output\n");
    kshell_printf(ctx, "\t\\e  escape\n");
    kshell_printf(ctx, "\t\\f  form feed\n");
    kshell_printf(ctx, "\t\\n  new line\n");
    kshell_printf(ctx, "\t\\r  carriage return\n");
    kshell_printf(ctx, "\t\\t  horizontal tab\n");
    kshell_printf(ctx, "\t\\v  vertical tab\n");
}

static int hextobin(unsigned char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (tolower(ch) >= 'a' && tolower(ch) <= 'f')
        return tolower(ch) - 'a' + 10;
    else
        return 0;
}

static state echo_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool nflag = false;
    bool eflag = false;
    bool iflag = false;
    char ch, tmp, *para;
    unsigned int index = 0;

    while (--argc) {
        para = *++argv;
        if (para[0] == '-' && para[1]) {
            switch (*++para) {
                case 'n':
                    nflag = true;
                    continue;

                case 'N':
                    nflag = false;
                    continue;

                case 'e':
                    eflag = true;
                    continue;

                case 'E':
                    eflag = false;
                    continue;

                case 'i':
                    iflag = true;
                    continue;

                case 'I':
                    iflag = false;
                    continue;

                case 'h':
                    usage(ctx);
                    return -ENOERR;
            }
        }
        break;
    }

    if (eflag) {
        while (argc--) {
            para = *argv++;
            while ((ch = *para++)) {
                if (ch == '\\' && *para) {
                    switch (ch = *para++) {
                        case 'a':
                            ch = '\a';
                            break;

                        case 'b':
                            ch = '\b';
                            break;

                        case 'c':
                            return -ENOERR;

                        case 'e':
                            ch = '\x1B';
                            break;

                        case 'f':
                            ch = '\f';
                            break;

                        case 'n':
                            ch = '\n';
                            break;

                        case 'r':
                            ch = '\r';
                            break;

                        case 't':
                            ch = '\t';
                            break;

                        case 'v':
                            ch = '\v';
                            break;

                        case 'x':
                            if (!isxdigit(tmp = *para++))
                                goto not_an_escape;

                            ch = hextobin(tmp);
                            tmp = *para;

                            if (isxdigit(tmp)) {
                                para++;
                                ch = ch * 16 + hextobin(tmp);
                            }
                            break;

                        case '0':
                            ch = 0;
                            if (! ('0' <= *para && *para <= '7'))
                                break;
                            ch = *para++;
                            fallthrough;

                        case '1': case '2': case '3':
                        case '4': case '5': case '6': case '7':
                            ch -= '0';
                            if ('0' <= *para && *para <= '7')
                                ch = ch * 8 + (*para++ - '0');
                            if ('0' <= *para && *para <= '7')
                                ch = ch * 8 + (*para++ - '0');
                            break;

                        case '\\':
                            break;

                        default: not_an_escape:
                            kshell_printf(ctx, "\\");
                            break;
                    }
                }
                if (iflag)
                    kshell_printf(ctx, "%u: ", index++);
                kshell_printf(ctx, "%c", ch);
            }
            if (argc) {
                if (iflag)
                    kshell_printf(ctx, "\n");
                else
                    kshell_printf(ctx, " ");
            }
        }
    }

    else while (argc--) {
        if (iflag)
            kshell_printf(ctx, "%u: ", index++);
        kshell_printf(ctx, "%s", *argv++);
        if (argc) {
            if (iflag)
                kshell_printf(ctx, "\n");
            else
                kshell_printf(ctx, " ");

        }
    }

    if (!nflag)
        kshell_printf(ctx, "\n");

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
