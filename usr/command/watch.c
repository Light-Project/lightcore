/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <console.h>
#include <delay.h>
#include <printk.h>

static void usage(void)
{
    printk("usage: watch [option] \"command\"                       \n");
    printk("  -n <millisecond>  millisecond to wait between updates \n");
    printk("  -t  turn off header message                           \n");
    printk("  -h  display this message                              \n");
}

static state delay_main(int argc, char *argv[])
{
    unsigned int delay = 500;
    unsigned int count, len;
    bool title = true;
    char *cmd, buff[10];
    state ret;

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

    cmd = argv[argc - 1];

    for (;;) {
        printk("\e[2J\e[1;1H");

        if (title)
            printk("Every %dms: %s\n\n", delay, cmd);

        ret = kshell_system(cmd);
        if (ret)
            return ret;

        for (count = 0; count < delay; ++count) {
            len = console_read(buff, 10);
            if (len && *buff == 'q')
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
