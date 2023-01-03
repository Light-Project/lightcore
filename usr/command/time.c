/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <timekeeping.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: time [option] commands ...\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state time_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    ktime_t start, end;
    state ret = -ENOERR;

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

    start = timekeeping_get_time();
    ret = kshell_execv(ctx, argv[count], argc - count, &argv[count]);
    end = timekeeping_get_time();

    kshell_printf(ctx, "total %llu.%06llums exec \"%s\"\n",
                  ktime_to_ms(ktime_sub(end, start)),
                  ktime_to_ns(ktime_sub(end, start)) % 1000000, argv[1]);

    return ret;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command time_cmd = {
    .name = "time",
    .desc = "show process run time",
    .exec = time_main,
};

static state time_init(void)
{
    return kshell_register(&time_cmd);
}
kshell_initcall(time_init);
