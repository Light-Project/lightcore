/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <string.h>
#include <initcall.h>
#include <kshell.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: for [option] varname in (start..end..step) {commands}\n");
    kshell_printf(ctx, "\t-v  generate an environment variable (default)\n");
    kshell_printf(ctx, "\t-V  does not generate environment variables\n");
    kshell_printf(ctx, "\t-x  hexadecimal format output\n");
    kshell_printf(ctx, "\t-X  decimal format output (default)\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

#define CHECK_PARAM {                       \
    if (!isdigit(*str) && *str != '-') {    \
        textmode = true;                    \
        break;                              \
    }                                       \
}

static state loop_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int index, count;
    char *varname;
    bool vflag = true;
    bool xflag = false;
    state retval = -ENOERR;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'v':
                vflag = true;
                break;

            case 'V':
                vflag = false;
                break;

            case 'x':
                xflag = true;
                break;

            case 'X':
                xflag = false;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc < count + (vflag ? 4 : 2))
        goto usage;

    if (vflag) {
        if (strcmp(argv[count + 1], "in"))
            goto usage;
        varname = argv[count + 0];
        count += 2;
    }

    for (index = count; index < argc - 1; ++index) {
        int var, start, end, step;
        bool textmode = false;
        const char *str, *tmp;
        char buff[32];

        start = end = 0;
        step = 1;

        for (var = 0, str = argv[index]; *str; var++, str = ++tmp) {
            if (*str == '(' && var == 0) {
                if (!(tmp = strstr(++str, ".."))) {
                    textmode = true;
                    break;
                }
                CHECK_PARAM
                start = axtoi(str);
            } else if (*str == '.' && var == 1) {
                if (!(tmp = strstr(++str, "..")) && !(tmp = strchr(str, ')'))) {
                    textmode = true;
                    break;
                }
                CHECK_PARAM
                end = axtoi(str);
            } else if (*str == '.' && var == 2) {
                if (!(tmp = strchr(++str, ')'))) {
                    textmode = true;
                    break;
                }
                CHECK_PARAM
                step = axtoi(str);
            } else {
                textmode = true;
                break;
            }
        }

        if (step == 0)
            textmode = true;

        else if (step < 0) {
            step = -step;
            swap(start, end);
        }

        if (textmode) {
            start = end = 0;
            step = 1;
        }

        for (var = start; start < end ? var <= end : var >= end; var += start < end ? step : -step) {
            if (vflag) {
                if (textmode)
                    kshell_setenv(ctx, varname, argv[index], true);
                else {
                    itoa(var, buff, xflag ? 16 : 10);
                    kshell_setenv(ctx, varname, buff, true);
                }
            }
            if (kshell_ctrlc(ctx))
                goto exit;
            retval = kshell_system(ctx, argv[argc -1]);
            if (retval && retval != -EAGAIN)
                goto exit;
            retval = -ENOERR;
        }
    }

exit:
    if (vflag)
        kshell_unsetenv(ctx, varname);

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command loop_cmd = {
    .name = "for",
    .desc = "circulates execute commands",
    .exec = loop_main,
};

static state loop_init(void)
{
    return kshell_register(&loop_cmd);
}
kshell_initcall(loop_init);
