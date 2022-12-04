/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <crash.h>
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

static state do_system(struct kshell_context *ctx, const char *cmdline)
{
    struct kshell_command *cmd;
    char **argv, retbuf[20];
    state retval = -ENOERR;
    int argc;

    while (cmdline && !ctx->breakdown) {
        retval = kshell_parser(ctx, &cmdline, &argc, &argv);
        if (retval)
            return retval;

        if (!argc)
            continue;

        if (strchr(argv[0], '=')) {
            kshell_putenv(ctx, argv[0]);
            continue;
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

        --*ctx->depth;
        retval = cmd->exec(ctx, argc, argv);
        ++*ctx->depth;
        kfree(argv);

        snprintf(retbuf, sizeof(retbuf), "%d", retval);
        kshell_symbol_set(ctx, "?", retbuf, true);

        if (ctx->tryrun && retval)
            break;
    }

    return retval;
}

state kshell_system(struct kshell_context *ctx, const char *cmdline)
{
    if (!cmdline) {
        kshell_printf(ctx, "kshell: command inval\n");
        return -EINVAL;
    }

    BUG_ON(ctx->breakdown);
    return do_system(ctx, cmdline);
}
EXPORT_SYMBOL(kshell_system);

state kshell_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct readline_state *rstate = ctx->readline;
    struct kshell_context nctx = {};
    unsigned int count;
    char *cmdline;
    state retval;

    context_clone(&nctx, ctx);
    retval = commands_prepare(&nctx, ctx);
    if (retval)
        return retval;

    if (abs(argc) != 1) {
        if (argc > 0)
            nctx.readline = NULL;
        else {
            argc = -argc;
            nctx.readline = rstate;
        }

        for (count = 1; !nctx.breakexit && count < argc; ++count)
            retval = do_system(&nctx, argv[count]);
    }

    else if (rstate) {
        rstate = readline_alloc(rstate->read, rstate->write, rstate->data);
        if (!(nctx.readline = rstate))
            return -ENOMEM;

        while (!nctx.breakexit) {
            cmdline = readline(rstate, kshell_getenv(&nctx, "PS1"), kshell_getenv(&nctx, "PS2"));
            if (!rstate->len)
                continue;
            nctx.breakdown = false;
            retval = do_system(&nctx, cmdline);
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

static state kshell_init(void)
{
    return kshell_register(&kshell_cmd) ||
           kshell_register(&help_cmd);
}
kshell_initcall(kshell_init);
