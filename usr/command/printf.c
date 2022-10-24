/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <ctype.h>
#include <string.h>
#include <kmalloc.h>
#include <cmdline.h>
#include <initcall.h>
#include <dynamic-call.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: printf [option] \"format\" ...\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state printf_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index = 2;
    void *printf_args[32] = {};
    const char *fmt, *walk;
    char *paraph;

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
    paraph = kmalloc(strlen(fmt) + 1, GFP_KERNEL);
    if (unlikely(!paraph))
        return -ENOMEM;

    escape_string(paraph, ~0, fmt);
    fmt = paraph;

    while ((fmt = strchr(fmt, '%'))) {
        walk = fmt; repeat:

        if (unlikely(index >= ARRAY_SIZE(printf_args))) {
            kshell_printf(ctx, "too many parameters\n");
            kfree(paraph);
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
        fmt = walk;
    }

    printf_args[0] = ctx;
    printf_args[1] = paraph;
    dynamic_call((dynamic_call_t)kshell_printf, index, printf_args);
    kfree(paraph);

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
