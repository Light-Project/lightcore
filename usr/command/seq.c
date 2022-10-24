/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <string.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: for [option] start end [step]\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state seq_main(struct kshell_context *ctx, int argc, char *argv[])
{
    int var, start, end, step;
    unsigned int count;
    char buff[32];

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (count == argc - 2) {
        start = axtoi(argv[count + 0]);
        end = axtoi(argv[count + 1]);
        step = 1;
    }

    else if (count == argc - 3) {
        start = axtoi(argv[count + 0]);
        end = axtoi(argv[count + 1]);
        step = axtoi(argv[count + 2]);
    }

    else
        goto usage;

    if (step < 0) {
        step = -step;
        swap(start, end);
    }

    for (var = start; start < end ? var <= end : var >= end; var += start < end ? step : -step) {
        itoad(var, buff);
        kshell_printf(ctx, "%s\n", buff);
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command seq_cmd = {
    .name = "seq",
    .desc = "print numbers from first to last",
    .exec = seq_main,
};

static state seq_init(void)
{
    return kshell_register(&seq_cmd);
}
kshell_initcall(seq_init);
