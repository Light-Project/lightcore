/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <errname.h>
#include <console.h>
#include <export.h>

static void context_clone(struct kshell_context *ctx, struct kshell_context *old)
{
    ctx->read = old->read;
    ctx->write = old->write;
    ctx->depth = old->depth;
}

static state commands_prepare(struct kshell_context *ctx, struct kshell_context *old)
{
    struct kshell_command *cmd;
    state ret = -ENOERR;

    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list) {
        if (cmd->prepare) {
            if ((ret = cmd->prepare(ctx, old)))
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

static state do_system(struct kshell_context *ctx, const char *cmdline, bool *exit)
{
    const char *buffer;
    char **argv;
    struct kshell_command *cmd;
    bool constant = true;
    state retval = -ENOERR;
    int argc;

    while (cmdline) {
        retval = kshell_parser(ctx, &buffer, &cmdline, &argc, &argv, &constant);
        if (retval)
            return retval;

        if (!argc)
            continue;

        if (exit && !strcmp(argv[0], "exit")) {
            *exit = true;
            kfree(argv);
            break;
        }

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
            break;
    }

    if (!constant)
        kfree(buffer);

    return retval;
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
    struct readline_state *rstate = ctx->readline;
    struct kshell_context nctx;
    char *cmdline, retbuf[20];
    unsigned int count;
    state retval = -ENOERR;
    bool exit = false;

    context_clone(&nctx, ctx);
    retval = commands_prepare(&nctx, ctx);
    if (retval)
        return retval;

    if (argc > 1) {
        nctx.readline = NULL;

        for (count = 1; !exit && count < argc; ++count) {
            retval = do_system(&nctx, argv[count], &exit);
            snprintf(retbuf, sizeof(retbuf), "%d", retval);
            kshell_setenv(&nctx, "?", retbuf, true);
            kshell_setenv(&nctx, "??", errname(retval) ?: "EUNKNOWN", true);
        }
    }

    else if (rstate) {
        rstate = readline_alloc(rstate->read, rstate->write, rstate->data);
        if (!(nctx.readline = rstate))
            return -ENOMEM;

        while (!exit) {
            cmdline = readline(rstate, kshell_getenv(&nctx, "PS1"), kshell_getenv(&nctx, "PS2"));
            if (!rstate->len)
                continue;

            retval = do_system(&nctx, cmdline, &exit);
            snprintf(retbuf, sizeof(retbuf), "%d", retval);
            kshell_setenv(&nctx, "?", retbuf, true);
            kshell_setenv(&nctx, "??", errname(retval) ?: "EUNKNOWN", true);
        }

        readline_free(rstate);
    }

    commands_release(&nctx);
    return retval;
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
        || kshell_register(&help_cmd)
        || kshell_register(&exit_cmd);
}
kshell_initcall(kshell_init);
