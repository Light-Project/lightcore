/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "params"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <types.h>
#include <string.h>
#include <ctype.h>
#include <param.h>
#include <initcall.h>
#include <printk.h>
#include <export.h>

char *param_next_arg(char *args, char **param, char **val)
{
    unsigned int count, equals = 0;
    bool in_quote = false, quoted = false;

    if (*args == '"') {
        in_quote = quoted = true;
        args++;
    }

    for (count = 0; args[count]; ++count) {
        if (isspace(args[count]) && !in_quote)
            break;
        if (!equals && args[count] == '=')
            equals = count;
        if (args[count] == '"')
            in_quote = !in_quote;
    }

    *param = args;

    if (!equals)
        *val = NULL;
    else {
        args[equals] = '\0';
        *val = args + equals + 1;
        if (**val == '"') {
            (*val)++;
            if (args[count - 1] == '"')
                args[count - 1] = '\0';
        }
    }

    if (quoted && args[count - 1] == '"')
        args[count - 1] = '\0';

    if (!args[count])
        args += count;
    else {
        args[count] = '\0';
        args += count + 1;
    }

    return skip_spaces(args);
}
EXPORT_SYMBOL(param_next_arg);

state param_parser(char *cmdline, param_entry_t entry, void *pdata)
{
    char *param, *val;
    state ret = -ENOERR;

    cmdline = skip_spaces(cmdline);
    if (!*cmdline)
        return -EINVAL;

    do {
        cmdline = param_next_arg(cmdline, &param, &val);

        /* Stop at -- */
        if (!val && !strcmp(param, "--"))
            return ret;

        ret = entry(param, val, pdata);
        switch (-ret) {
            case ENOERR:
                continue;

            case ENOENT:
                pr_err("Illegal instruction '%s'\n", param);
                break;

            case ENOSPC:
                pr_err("Value '%s' too large for '%s'\n", val ?: "", param);
                break;

            default:
                pr_err("Value '%s' invalid for '%s'\n", val ?: "", param);
                break;
        }
    } while (*cmdline);

    return ret;
}
EXPORT_SYMBOL(param_parser);
