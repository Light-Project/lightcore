/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <ctype.h>
#include <string.h>
#include <cmdline.h>
#include <initcall.h>
#include <dynamic-call.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: printf [option] \"format\" ...\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state printf_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index = 2;
    void *printf_args[32] = {};
    char *fmt, *walk;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h': default:
                goto usage;
        }
    }

    if (count >= argc)
        goto usage;

    fmt = argv[count++];
    escape_string(fmt, ~0, fmt);

    for (walk = fmt; (walk = strchr(walk, '%'));) {
        repeat:

        if (unlikely(index >= ARRAY_SIZE(printf_args))) {
            kshell_puts(ctx, "too many parameters\n");
            return -EFBIG;
        }

        switch (*++walk) {
            case 'd': case 'i':
                if (count >= argc)
                    goto usage;
                printf_args[index++] =
                (void *)(uintptr_t)axtol(argv[count++]);
                break;

            case 'u': case 'o': case 'x':
            case 'X': case 'p': case 'P':
                if (count >= argc)
                    goto usage;
                printf_args[index++] =
                (void *)(uintptr_t)axtoul(argv[count++]);
                break;

            case 's':
                if (count >= argc)
                    goto usage;
                printf_args[index++] =
                (void *)(uintptr_t)argv[count++];
                break;

            case 'c':
                if (count >= argc)
                    goto usage;
                printf_args[index++] =
                (void *)(uintptr_t)*argv[count++];
                break;

            case '*':
                if (count >= argc)
                    goto usage;
                printf_args[index++] =
                (void *)(uintptr_t)axtoul(argv[count++]);
                goto repeat;

            default:
                if (isalpha(*walk) || !*walk) {
                    kshell_printf(ctx, "invalid directive '%.*s'\n", walk - fmt + 1, fmt);
                    return -EINVAL;
                }
                goto repeat;
        }
    }

    printf_args[0] = ctx;
    printf_args[1] = fmt;
    dynamic_call((dynamic_call_t)kshell_printf, index, printf_args);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command printf_cmd = {
    .name = "printf",
    .desc = "format and print data",
    .exec = printf_main,
};

static state printf_init(void)
{
    return kshell_register(&printf_cmd);
}
kshell_initcall(printf_init);
