/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <ctype.h>
#include <string.h>

#define WC_FLAG_LINE  BIT(0)
#define WC_FLAG_WORD  BIT(1)
#define WC_FLAG_CHAR  BIT(2)
#define WC_FLAG_ZERO  BIT(4)

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: wc [option] string...\n");
    kshell_printf(ctx, "\t-l  print the newline counts\n");
    kshell_printf(ctx, "\t-w  print the word counts\n");
    kshell_printf(ctx, "\t-c  print the byte counts\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state wc_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, lcnt, wcnt, ccnt;
    uint8_t flags = 0xff;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        if (flags & WC_FLAG_ZERO)
            flags = 0;

        switch (para[1]) {
            case 'l':
                flags |= WC_FLAG_LINE;
                break;

            case 'w':
                flags |= WC_FLAG_WORD;
                break;

            case 'c':
                flags |= WC_FLAG_CHAR;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    for (lcnt = wcnt = ccnt = 0; count < argc; ++count) {
        const char *walk;

        for (walk = argv[count]; flags & WC_FLAG_CHAR && *walk; ++walk)
            ccnt++;

        for (walk = argv[count]; flags & WC_FLAG_WORD && *walk; ++walk) {
            if (isalnum(walk[0]) && !isalnum(walk[1]))
                wcnt++;
        }

        for (walk = argv[count]; flags & WC_FLAG_LINE && *walk; ++walk) {
            if (*walk == '\n')
                lcnt++;
        }
    }

    if (flags & WC_FLAG_LINE)
        kshell_printf(ctx, "%u\t", lcnt);

    if (flags & WC_FLAG_WORD)
        kshell_printf(ctx, "%u\t", wcnt);

    if (flags & WC_FLAG_CHAR)
        kshell_printf(ctx, "%u\t", ccnt);

    kshell_putc(ctx, '\n');
    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command wc_cmd = {
    .name = "wc",
    .desc = "statistics string information",
    .exec = wc_main,
};

static state wc_init(void)
{
    return kshell_register(&wc_cmd);
}
kshell_initcall(wc_init);
