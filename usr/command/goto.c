/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

static void usage(void)
{
    kshell_printf("usage: goto [option] [address]...\n");
    kshell_printf("\t-h  display this message\n");
}

static int goto_main(int argc, char *argv[])
{
    state (*program)(int argc, char *argv[]);
    unsigned int count;
    state retval;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    program = (void *)strtoul(argv[count]);
    if (!program)
        goto usage;

    kshell_printf("Goto program at (%p)\n", program);
    retval = program(argc - count - 1, &argv[count + 1]);
    kshell_printf("Program terminated, retval (%d)\n", retval);

    return retval;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command goto_cmd = {
    .name = "goto",
    .desc = "goto specifies address program",
    .exec = goto_main,
};

static state goto_init(void)
{
    return kshell_register(&goto_cmd);
}
kshell_initcall(goto_init);
