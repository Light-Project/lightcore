/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <string.h>
#include <initcall.h>
#include <kshell.h>

static void usage(void)
{
    kshell_printf("usage: loop [option] varname in {min..max..step} 'commands'\n");
    kshell_printf("\t-v  generate an environment variable (default)\n");
    kshell_printf("\t-V  does not generate environment variables\n");
    kshell_printf("\t-x  hexadecimal format output\n");
    kshell_printf("\t-X  decimal format output (default)\n");
}

static state loop_main(int argc, char *argv[])
{
    unsigned int var, count, min, max, step = 1;
    char *varname, *str, *tmp;
    char buff[32];
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

    if (argc != count + (vflag ? 4 : 2))
        goto usage;

    if (vflag) {
        if (strcmp(argv[count + 1], "in"))
            goto usage;
        varname = argv[count + 0];
        count += 2;
    }

    for (var = 0, str = argv[count]; *str; var++, str = ++tmp) {
        if (*str == '{' && var == 0) {
            if (!(tmp = strstr(++str, "..")))
                goto usage;
            if (!isdigit(*str))
                goto usage;
            *tmp = '\0';
            min = strtoul(str);
        } else if (*str == '.' && var == 1) {
            if (!(tmp = strstr(++str, "..")) && !(tmp = strchr(str, '}')))
                goto usage;
            if (!isdigit(*str))
                goto usage;
            *tmp = '\0';
            max = strtoul(str);
        } else if (*str == '.' && var == 2) {
            if (!(tmp = strchr(++str, '}')))
                goto usage;
            if (!isdigit(*str))
                goto usage;
            *tmp = '\0';
            step = strtoul(str);
        } else
            goto usage;
    }

    for (var = min; var <= max; var += step) {
        if (vflag) {
            itoa(var, buff, xflag ? 16 : 10);
            kshell_setenv(varname, buff, true);
        }
        if (kshell_ctrlc())
            break;
        retval = kshell_system(argv[count + 1]);
        if (retval)
            break;
    }

    if (vflag)
        kshell_unsetenv(varname);

    return retval;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command loop_cmd = {
    .name = "loop",
    .desc = "circulates execute commands",
    .exec = loop_main,
};

static state loop_init(void)
{
    return kshell_register(&loop_cmd);
}
kshell_initcall(loop_init);
