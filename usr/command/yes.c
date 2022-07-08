/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: yes [option] [string]...\n");
    kshell_printf(ctx, "\t-t  <time> total output times\n");
    kshell_printf(ctx, "\t-n  not output the trailing newline\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state yes_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, time = 0;
    unsigned int walk;
    bool nflag = false;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 't':
                if ((++count) >= argc)
                    goto usage;
                time = atoi(argv[count]);
                break;

            case 'n':
                nflag = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    do {
        if (kshell_ctrlc(ctx))
            break;
        kshell_printf(ctx, "%s", argv[count]);
        for (walk = count + 1; walk < argc; ++walk)
            kshell_printf(ctx, " %s", argv[walk]);
        if (!nflag)
            kshell_printf(ctx, "\n");
    } while (--time);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command yes_cmd = {
    .name = "yes",
    .desc = "loop printf of specific string",
    .exec = yes_main,
};

static state yes_init(void)
{
    return kshell_register(&yes_cmd);
}
kshell_initcall(yes_init);

