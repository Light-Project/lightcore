/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <panic.h>

enum panic_type {
    PANIC_WARN      = 0,
    PANIC_BUG       = 1,
    PANIC_PANIC     = 2,
};

static void usage(void)
{
    kshell_printf("usage: panic [option] \"message\"\n");
    kshell_printf("\t-w  manually trigger a warn\n");
    kshell_printf("\t-b  manually trigger a bug\n");
    kshell_printf("\t-p  manually trigger a bug\n");
    kshell_printf("\t-h  display this message\n");
}

static state panic_main(int argc, char *argv[])
{
    enum panic_type type = PANIC_PANIC;
    unsigned int count;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'w':
                type = PANIC_WARN;
                break;

            case 'b':
                type = PANIC_BUG;
                break;

            case 'p':
                type = PANIC_PANIC;
                break;

            case 'h': default:
                goto usage;
        }
    }

    switch (type) {
        case PANIC_WARN:
            WARN();
            break;

        case PANIC_BUG:
            BUG();
            break;

        case PANIC_PANIC:
            panic("%s", argv[1]);
            unreachable();
    }

    return -ENOERR;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command panic_cmd = {
    .name = "panic",
    .desc = "panic trigger",
    .exec = panic_main,
};

static state panic_init(void)
{
    return kshell_register(&panic_cmd);
}
kshell_initcall(panic_init);
