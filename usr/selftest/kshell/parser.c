/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "../../kshell/kshell.h"
#include <string.h>
#include <initcall.h>
#include <selftest.h>
#include <printk.h>

static const char parser_string[] = {
    "TEST " "'QUOTE' " "\"DQUOTE\" "
    "$TEST_VARNAME " "${TEST_LVARNAME} "
    "\"$TEST_QVARNAME\" " "\"${TEST_LQVARNAME}\" "
    ";ESC"
};

static state parser_setenv(void)
{
    return kshell_putenv("TEST_VARNAME=VARNAME")
        || kshell_putenv("TEST_LVARNAME=LVARNAME")
        || kshell_putenv("TEST_QVARNAME=QVARNAME")
        || kshell_putenv("TEST_LQVARNAME=LQVARNAME");
}

static void parser_unsetenv(void)
{
    kshell_unsetenv("TEST_VARNAME");
    kshell_unsetenv("TEST_LVARNAME");
    kshell_unsetenv("TEST_QVARNAME");
    kshell_unsetenv("TEST_LQVARNAME");
}

static state parser_testing(void *pdata)
{
    const char *cmdline;
    char **argv;
    int argc;
    state retval;

    if ((retval= parser_setenv()))
        goto exit;

    retval = kshell_parser(parser_string, &cmdline, &argc, &argv);
    if (retval)
        goto exit;

    printk("TEST: %s\n",            argv[0]);
    printk("QUOTE: %s\n",           argv[1]);
    printk("DQUOTE: %s\n",          argv[2]);
    printk("TEST_VARNAME: %s\n",    argv[3]);
    printk("TEST_LVARNAME: %s\n",   argv[4]);
    printk("TEST_QVARNAME: %s\n",   argv[5]);
    printk("TEST_LQVARNAME: %s\n",  argv[6]);

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

    retval = kshell_parser(cmdline, &cmdline, &argc, &argv);
    if (retval)
        goto exit;

    printk("ESC: %s\n", argv[0]);

    if (strcmp(argv[0], "ESC"))
        retval = -EINVAL;

exit:
    parser_unsetenv();
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
