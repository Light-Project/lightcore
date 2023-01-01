/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <logo.h>

#define VERSION_FLAG_TLOGO BIT(0)
#define VERSION_FLAG_HNAME BIT(1)
#define VERSION_FLAG_KRELE BIT(2)

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: verison [option]...\n");
    kshell_printf(ctx, "\t-l  print the terminal logo\n");
    kshell_printf(ctx, "\t-n  print the host name\n");
    kshell_printf(ctx, "\t-r  print the kernel release\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state version_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned long flags = 0;
    unsigned int count;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'a':
                flags = ULONG_MAX;
                break;

            case 'l':
                flags |= VERSION_FLAG_TLOGO;
                break;

            case 'n':
                flags |= VERSION_FLAG_HNAME;
                break;

            case 'r':
                flags |= VERSION_FLAG_KRELE;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (!flags)
        flags = VERSION_FLAG_HNAME;

    if (flags & VERSION_FLAG_TLOGO)
        kshell_printf(ctx, "%s", terminal_logo);

    if (flags & VERSION_FLAG_HNAME)
        kshell_printf(ctx, "%s ", CONFIG_DEFAULT_HOSTNAME);

    if (flags & VERSION_FLAG_KRELE)
        kshell_printf(ctx, "%s", CONFIG_VERSION);

    kshell_putc(ctx, '\n');

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command version_cmd = {
    .name = "version",
    .desc = "print certain system information",
    .exec = version_main,
};

static state version_init(void)
{
    return kshell_register(&version_cmd);
}
kshell_initcall(version_init);
