/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <console.h>
#include <delay.h>
#include <driver/buzzer.h>

#define FLAG_BEEPNZ     BIT(0)
#define FLAG_EXITNZ     BIT(1)
#define FLAG_NOHEAD     BIT(2)

#define BELL_PITCH      750
#define BELL_DURATION   100

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: watch [option] command\n");
    kshell_printf(ctx, "\t-n  <millisecond> millisecond to wait between updates\n");
    kshell_printf(ctx, "\t-b  beep if command has a non-zero exit");
    kshell_printf(ctx, "\t-e  exit if command has a non-zero exit");
    kshell_printf(ctx, "\t-t  turn off header message\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state watch_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, delay = 500;
    unsigned int flags = 0;
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

            case 'b':
                flags |= FLAG_BEEPNZ;
                break;

            case 'e':
                flags |= FLAG_EXITNZ;
                break;

            case 't':
                flags |= FLAG_NOHEAD;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    for (;;) {
        kshell_printf(ctx, "\e[2J\e[1;1H");
        if (!(flags & FLAG_NOHEAD))
            kshell_printf(ctx, "Every %dms: %s\n\n", delay, argv[count]);

        retval = kshell_execv(ctx, argv[count], argc - count, &argv[count]);
        if (unlikely(retval)) {
            if (flags & FLAG_BEEPNZ)
                buzzer_beep(READ_ONCE(default_buzzer), BELL_PITCH, BELL_DURATION);
            if (flags & FLAG_EXITNZ)
                return retval;
        }

        if (kshell_ctrlc(ctx))
            return -ECANCELED;

        msleep(delay);
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command watch_cmd = {
    .name = "watch",
    .desc = "execute a program periodically",
    .exec = watch_main,
};

static state watch_init(void)
{
    return kshell_register(&watch_cmd);
}
kshell_initcall(watch_init);
