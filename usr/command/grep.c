/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <textsearch.h>
#include <glob.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: grep [option]... patterns [strings]...\n");
    kshell_printf(ctx, "\t-g  patterns are basic regular expressions\n");
    kshell_printf(ctx, "\t-f  patterns are fixed strings\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state grep_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index, length;
    bool fixed = false;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'g':
                fixed = false;
                break;

            case 'f':
                fixed = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc < count + 2)
        goto usage;

    if (fixed) {
        struct ts_linear linear;
        struct ts_context *tsc;

        length = strlen(argv[count]);
        tsc = textsearch_prepare("kmp", argv[count], length, GFP_KERNEL, 0);
        if (IS_INVAL(tsc))
            return PTR_INVAL(tsc);

        for (index = count + 1; index < argc; ++index) {
            length = strlen(argv[index]);
            if (textsearch_linear_find(tsc, &linear, argv[index], length) != UINT_MAX)
                kshell_printf(ctx, "%s\n", argv[index]);
        }

        textsearch_destroy(tsc);
    } else {
        for (index = count + 1; index < argc; ++index) {
            if (glob_match(argv[count], argv[index]))
                kshell_printf(ctx, "%s\n", argv[index]);
        }
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command grep_cmd = {
    .name = "grep",
    .desc = "glob matching string",
    .exec = grep_main,
};

static state grep_init(void)
{
    return kshell_register(&grep_cmd);
}
kshell_initcall(grep_init);
