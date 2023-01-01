/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <string.h>
#include <initcall.h>
#include <kshell.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME head
# define PROGRAM_DESC "print head lines"
# define PROGRAM_FUNC(string, lines, chars) ({  \
    char *tmp, *walk = string;                  \
    while (lines--) {                           \
        if (!(tmp = strchr(walk, '\n')))        \
            goto finish;                        \
        walk = tmp + 1;                         \
    }                                           \
    if (chars)                                  \
        walk += min(strlen(string), chars);     \
    *walk = '\0';                               \
    finish: string;                             \
})
#else
# define PROGRAM_DESC "print tail lines"
# define PROGRAM_FUNC(string, lines, chars) ({  \
    char *walk = string + strlen(string);       \
    if (lines) while (string != walk) {         \
        if (*walk-- != '\n' &&                  \
            *walk == '\n' && !--lines)          \
            goto finish;                        \
    }                                           \
    if (chars)                                  \
        walk -= min(strlen(string), chars);     \
    finish: walk;                               \
})
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: "__stringify(PROGRAM_NAME)" [option] [string]\n");
    kshell_printf(ctx, "\t-l  <lines> total output line\n");
    kshell_printf(ctx, "\t-c  <chars> total output char\n");
    kshell_printf(ctx, "\t-n  not output the trailing newline\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state head_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int lines = 0, chars = 0;
    unsigned int count;
    char *string;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'l':
                if ((++count) >= argc)
                    goto usage;
                lines = atoi(argv[count]);
                chars = 0;
                break;

            case 'c':
                if ((++count) >= argc)
                    goto usage;
                chars = atoi(argv[count]);
                lines = 0;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count + 1)
        goto usage;

    if (!lines && !chars)
        lines = 10;

    string = PROGRAM_FUNC(argv[count], lines, chars);
    kshell_puts(ctx, string);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command head_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = head_main,
};

static state head_init(void)
{
    return kshell_register(&head_cmd);
}
kshell_initcall(head_init);
