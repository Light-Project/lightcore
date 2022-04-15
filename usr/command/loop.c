/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <initcall.h>
#include <kshell.h>

static void usage(void)
{
    kshell_printf("usage: loop [option] [var min max] 'commands'\n");
    kshell_printf("\t-v  generate an environment variable (default)\n");
    kshell_printf("\t-V  does not generate environment variables\n");
}

static state loop_main(int argc, char *argv[])
{
    unsigned int var, count, min, max;
    const char *varname;
    char buff[32];
    bool vflag = true;

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

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count + 4)
        goto usage;

    varname = argv[count + 0];
    min = strtoul(argv[count + 1]);
    max = strtoul(argv[count + 2]);

    for (var = min; var <= max; ++var) {
        if (vflag) {
            itoa(var, buff, 10);
            kshell_setenv(varname, buff, true);
        }
        if (kshell_ctrlc())
            break;
        kshell_system(argv[count + 3]);
    }

    if (vflag)
        kshell_unsetenv(varname);

    return -ENOERR;

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
