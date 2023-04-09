/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>

#define FLAG_MULT  BIT(0)
#define FLAG_NULL  BIT(1)

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: basename [option] name [suffix] ...\n");
    kshell_puts(ctx, "\t-a  support multiple arguments and treat each as a NAME\n");
    kshell_puts(ctx, "\t-s  remove a trailing suffix; implies -a\n");
    kshell_puts(ctx, "\t-z  end each output line with NUL, not newline\n");
}

static void remove_suffix(char *name, const char *suffix)
{
    const char *smatch;
    char *nmatch;

    nmatch = name + strlen(name);
    smatch = suffix + strlen(suffix);

    while (nmatch > name && smatch > suffix) {
        if (*--nmatch != *--smatch)
            return;
    }

    if (nmatch > name)
        *nmatch = '\0';
}

static void perform_basename(struct kshell_context *ctx, char *name,
                             const char *suffix, unsigned int flags)
{
    name = basename(name);
    if (suffix)
        remove_suffix(name, suffix);

    kshell_puts(ctx, name);
    if (!(flags & FLAG_NULL))
        kshell_putc(ctx, '\n');
}

static state basename_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, flags = 0;
    const char *suffix = NULL;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 's':
                if ((++count) >= argc)
                    goto usage;
                suffix = argv[count];
                fallthrough;

            case 'a':
                flags |= FLAG_MULT;
                break;

            case 'z':
                flags |= FLAG_NULL;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    if (!(flags & FLAG_MULT))
        perform_basename(ctx, argv[count], argv[count + 1], flags);
    else {
        do
            perform_basename(ctx, argv[count], suffix, flags);
        while (++count < argc);
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command basename_cmd = {
    .name = "basename",
    .desc = "print name with any leading directory components removed",
    .exec = basename_main,
};

static state basename_init(void)
{
    return kshell_register(&basename_cmd);
}
kshell_initcall(basename_init);
