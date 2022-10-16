/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <glob.h>
#include <string.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: switch [option] condition case 'cmp' {commands}...\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state switch_main(struct kshell_context *ctx, int argc, char *argv[])
{
    state retval = -ENOENT;
    unsigned int count;
    char *condition;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h':
                goto usage;

            default:
                goto finish;
        }
    }

finish:
    if (argc < count + 4)
        goto usage;

    condition = argv[count++];
    for (; count < argc; count += 2) {
        if (!strcmp("case", argv[count])) {
            if (count > argc - 3)
                goto usage;
            if (!glob_match(argv[++count], condition))
                continue;
        } else if (!strcmp("default", argv[count])) {
            if (count > argc - 2)
                goto usage;
        } else
            goto usage;

        retval = kshell_system(ctx, argv[count + 1]);
        if (!argv[count + 2] || strcmp("fallthrough", argv[count + 2]))
            break;

        count++;
    }

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command switch_cmd = {
    .name = "switch",
    .desc = "multi condition judgment",
    .exec = switch_main,
};

static state switch_init(void)
{
    return kshell_register(&switch_cmd);
}
kshell_initcall(switch_init);
