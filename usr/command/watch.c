/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <console.h>
#include <delay.h>

static void usage(void)
{
    kshell_printf("usage: watch [option] command\n");
    kshell_printf("\t-n  <millisecond>  millisecond to wait between updates \n");
    kshell_printf("\t-t  turn off header message\n");
    kshell_printf("\t-h  display this message\n");
}

static state delay_main(int argc, char *argv[])
{
    unsigned int tmp, delay = 500;
    unsigned int count;
    bool title = true;
    state retval;

    if (argc < 2)
        goto usage;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'n':
                if ((++count) >= argc)
                    goto usage;
                delay = atoi(argv[count]);
                if (!delay)
                    goto usage;
                break;

            case 't':
                title = false;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    for (;;) {
        kshell_printf("\e[2J\e[1;1H");

        if (title)
            kshell_printf("Every %dms: %s\n\n", delay, argv[count]);

        retval = kshell_execv(argv[count], argc - count, &argv[count]);
        if (retval)
            return retval;

        for (tmp = 0; tmp < delay; ++tmp) {
            if (kshell_ctrlc())
                goto exit;
            mdelay(1);
        }
    }

exit:
    return -ENOERR;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command watch_cmd = {
    .name = "watch",
    .desc = "execute a program periodically",
    .exec = delay_main,
};

static state watch_init(void)
{
    return kshell_register(&watch_cmd);
}
kshell_initcall(watch_init);
