/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "../../kshell/kshell.h"
#include <kmalloc.h>
#include <string.h>
#include <initcall.h>
#include <selftest.h>

static const char parser_string[] = {
    "TEST " "'QUOTE' " "\"DQUOTE\" "
    "$TEST_VARNAME " "${TEST_LVARNAME} "
    "\"$TEST_QVARNAME\" " "\"${TEST_LQVARNAME}\" "
    ";ESC"
};

static state parser_setenv(struct kshell_context *ctx)
{
    return kshell_putenv(ctx, "TEST_VARNAME=VARNAME")
        || kshell_putenv(ctx, "TEST_LVARNAME=LVARNAME")
        || kshell_putenv(ctx, "TEST_QVARNAME=QVARNAME")
        || kshell_putenv(ctx, "TEST_LQVARNAME=LQVARNAME");
}

static void parser_unsetenv(struct kshell_context *ctx)
{
    kshell_unsetenv(ctx, "TEST_VARNAME");
    kshell_unsetenv(ctx, "TEST_LVARNAME");
    kshell_unsetenv(ctx, "TEST_QVARNAME");
    kshell_unsetenv(ctx, "TEST_LQVARNAME");
}

static state parser_testing(struct kshell_context *ctx, void *pdata)
{
    bool constant = true;
    const char *cmdline = parser_string;
    const char **buffer = &cmdline;
    char **argv;
    int argc;
    state retval;

    if ((retval= parser_setenv(ctx)))
        goto exit;

    retval = kshell_parser(ctx, buffer, &cmdline, &argc, &argv, &constant);
    if (retval)
        goto exit;

    kshell_printf(ctx, "TEST: %s\n",            argv[0]);
    kshell_printf(ctx, "QUOTE: %s\n",           argv[1]);
    kshell_printf(ctx, "DQUOTE: %s\n",          argv[2]);
    kshell_printf(ctx, "TEST_VARNAME: %s\n",    argv[3]);
    kshell_printf(ctx, "TEST_LVARNAME: %s\n",   argv[4]);
    kshell_printf(ctx, "TEST_QVARNAME: %s\n",   argv[5]);
    kshell_printf(ctx, "TEST_LQVARNAME: %s\n",  argv[6]);

    if (strcmp(argv[0], "TEST") ||
        strcmp(argv[1], "QUOTE") ||
        strcmp(argv[2], "DQUOTE") ||
        strcmp(argv[3], "VARNAME") ||
        strcmp(argv[4], "LVARNAME") ||
        strcmp(argv[5], "QVARNAME") ||
        strcmp(argv[6], "LQVARNAME")) {
        retval = -EINVAL;
        goto exit;
    }
    kfree(argv);

    retval = kshell_parser(ctx, buffer, &cmdline, &argc, &argv, &constant);
    if (retval)
        goto exit;

    kshell_printf(ctx, "ESC: %s\n", argv[0]);

    if (strcmp(argv[0], "ESC"))
        retval = -EINVAL;
    kfree(argv);

    if (!constant)
        kfree(*buffer);

exit:
    parser_unsetenv(ctx);
    return retval;
}

static struct selftest_command parser_command = {
    .group = "kshell",
    .name = "parser",
    .desc = "parser unit test",
    .testing = parser_testing,
};

static state selftest_parser_init(void)
{
    return selftest_register(&parser_command);
}
kshell_initcall(selftest_parser_init);
