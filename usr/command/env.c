/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>

#ifndef PROGRAM_NAME
# define PROGRAM_NAME env
# define PROGRAM_DESC "setting environment variables"
# define PROGRAM_FUNC kshell_global
# define PROGRAM_VARS
# define PROGRAM_CHILD &ctx->env
#endif

static void usage(struct kshell_context *ctx, const char *cmd)
{
    kshell_puts(ctx, "usage: "__stringify(PROGRAM_NAME)" [option] name[=value]...\n");
    kshell_puts(ctx, "\t-u  remove variable from the environment\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state cmd_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_env *env = NULL;
    state retval = -ENOERR;
    unsigned int count;
    PROGRAM_VARS;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'l':
                env = (void *)true;
                break;

            case 'u':
                if ((++count) >= argc)
                    goto usage;
                __PASTE(PROGRAM_FUNC, _unset)(ctx, argv[count]);
                break;

            case 'h': default:
                goto usage;
        }
    }

    while (count < argc) {
        retval = __PASTE(PROGRAM_FUNC, _put)(ctx, argv[count]);
        if (retval == -EINVAL)
            kshell_printf(ctx, __stringify(cmd)
                          ": not a identifier: %s\n", argv[count]);
        count++;
        if (retval)
            break;
    }

    if (count == 1 || (env && !retval)) {
        rb_for_each_entry(env, PROGRAM_CHILD, node)
            kshell_printf(ctx, "\t%s=%s\n", env->name, env->val);
    }

    return retval;

usage:
    usage(ctx, argv[0]);
    return -EINVAL;
}

static struct kshell_command kshell_env_cmd = {
    .name = __stringify(PROGRAM_NAME),
    .desc = PROGRAM_DESC,
    .exec = cmd_main,
};

static state env_init(void)
{
    return kshell_register(&kshell_env_cmd);
}
kshell_initcall(env_init);
