/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <setjmp.h>
#include <kmalloc.h>
#include <initcall.h>
#include <console.h>
#include <export.h>

static void context_clone(struct kshell_context *ctx, struct kshell_context *new)
{
    new->env = RB_INIT;
    new->read = ctx->read;
    new->write = ctx->write;
    new->depth = ctx->depth;
    kshell_envclone(ctx, new);
}

static state commands_prepare(struct kshell_context *ctx)
{
    struct kshell_command *cmd;
    state ret = -ENOERR;

    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list) {
        if (cmd->prepare) {
            if ((ret = cmd->prepare(ctx)))
                break;
        }
    }
    spin_unlock(&kshell_lock);

    return ret;
}

static void commands_release(struct kshell_context *ctx)
{
    struct kshell_command *cmd;

    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list) {
        if (cmd->release)
            cmd->release(ctx);
    }
    spin_unlock(&kshell_lock);
}

static state do_system(struct kshell_context *ctx, char *cmdline, jmp_buf *buff)
{
    struct kshell_command *cmd;
    state retval;
    char **argv;
    int argc;

    while (cmdline) {
        retval = kshell_parser(ctx, cmdline, (const char **)&cmdline, &argc, &argv);
        if (retval)
            return retval;

        if (!argc)
            continue;

        if (buff && !strcmp(argv[0], "exit"))
            longjmp(buff, true);

        if (!*ctx->depth) {
            kshell_printf(ctx, "kshell: trigger recursive protection\n");
            return -EFBIG;
        }

        cmd = kshell_find(argv[0]);
        if (!cmd) {
            kshell_printf(ctx, "kshell: command not found: %s\n", argv[0]);
            kfree(argv);
            return -EBADF;
        }

        if (!cmd->exec)
            return -ENXIO;

        --*ctx->depth;
        retval = cmd->exec(ctx, argc, argv);
        ++*ctx->depth;
        kfree(argv);

        if (retval)
            return retval;
    }

    return -ENOERR;
}

state kshell_system(struct kshell_context *ctx, const char *cmdline)
{
    char *ncmdline;
    state ret;

    if (!cmdline) {
        kshell_printf(ctx, "kshell: command inval\n");
        return -EINVAL;
    }

    ncmdline = strdup(cmdline);
    ret = do_system(ctx, ncmdline, NULL);
    kfree(ncmdline);

    return ret;
}
EXPORT_SYMBOL(kshell_system);

state kshell_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_context nctx;
    char *cmdline, retbuf[20];
    state exit, ret = -ENOERR;
    unsigned int count;
    jmp_buf buff;

    context_clone(ctx, &nctx);
    exit = commands_prepare(&nctx);
    if (exit)
        return exit;

    if (argc > 1) {
        exit = setjmp(&buff);
        nctx.readline = NULL;

        for (count = 1; !exit && count < argc; ++count) {
            ret = do_system(&nctx, argv[count], &buff);
            snprintf(retbuf, sizeof(retbuf), "%d", ret);
            kshell_setenv(&nctx, "?", retbuf, true);
        }
    }

    else if (ctx->readline) {
        nctx.readline = readline_alloc(ctx->readline->read, ctx->readline->write, NULL);
        if (!nctx.readline)
            return -ENOMEM;

        exit = setjmp(&buff);

        while (!exit) {
            cmdline = readline(nctx.readline, "kshell: /# ");
            if (!nctx.readline->len)
                continue;
            ret = do_system(&nctx, cmdline, &buff);
            snprintf(retbuf, sizeof(retbuf), "%d", ret);
            kshell_setenv(&nctx, "?", retbuf, true);
        }

        readline_free(nctx.readline);
    }

    kshell_envrelease(&nctx);
    commands_release(&nctx);

    return ret;
}

static state env_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_env *env;
    rb_for_each_entry(env, &ctx->env, node)
        kshell_printf(ctx, "\t%s=%s\n", env->name, env->val);
    return -ENOERR;
}

static state help_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_command *cmd;
    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list)
        kshell_printf(ctx, "\t%-16s - %s\n", cmd->name, cmd->desc);
    spin_unlock(&kshell_lock);
    return -ENOERR;
}

static struct kshell_command kshell_cmd = {
    .name = "kshell",
    .desc = "kshell interpreter",
    .exec = kshell_main,
};

static struct kshell_command env_cmd = {
    .name = "env",
    .desc = "displays all environment variable",
    .exec = env_main,
};

static struct kshell_command help_cmd = {
    .name = "help",
    .desc = "displays all available instructions",
    .exec = help_main,
};

static struct kshell_command exit_cmd = {
    .name = "exit",
    .desc = "exit the kshell interpreter",
};

static state kshell_init(void)
{
    return kshell_register(&kshell_cmd)
        || kshell_register(&env_cmd)
        || kshell_register(&help_cmd)
        || kshell_register(&exit_cmd);
}
kshell_initcall(kshell_init);
