/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <asm/regs.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: x86-tsc [option]\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state tsc_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    uint64_t value;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (argc != count)
        goto usage;

    value = tsc_get();
    kshell_printf(ctx, "rdtsc: %#018llx\n", value);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command tsc_cmd = {
    .name = "x86-tsc",
    .desc = "show processor tsc",
    .exec = tsc_main,
};

static state tsc_init(void)
{
    return kshell_register(&tsc_cmd);
}
kshell_initcall(tsc_init);
