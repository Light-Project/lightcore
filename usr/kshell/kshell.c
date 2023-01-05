/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <stdlib.h>
#include <cmdline.h>
#include <crash.h>
#include <kmalloc.h>
#include <initcall.h>
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

        if (!ctx->depth) {
            kshell_puts(ctx, "trigger recursive protection\n");
            return -EFBIG;
        }

        cmd = kshell_find(argv[0]);
        if (!cmd) {
            kshell_printf(ctx, "command not found: %s\n", argv[0]);

            cmd = kshell_closest(argv[0]);
            if (cmd)
                kshell_printf(ctx, "most similar command is: %s\n", cmd->name);

            retval = -EBADF;
        } else {
            ctx->depth--;
            retval = cmd->exec(ctx, argc, argv);
            ctx->depth++;
        }

        kfree(argv);

        intoa(retval, retbuf, 10, sizeof(retbuf));
        kshell_symbol_set(ctx, "?", retbuf, true);

        intoa(errno, retbuf, 10, sizeof(retbuf));
        kshell_symbol_set(ctx, "!", retbuf, true);

        if (ctx->tryrun) {
            if (unlikely(retval))
                break;
            else if (unlikely(errno)) {
                retval = errno;
                break;
            }
        }
    }

    return retval;
}

state kshell_system(struct kshell_context *ctx, const char *cmdline)
{
    if (!cmdline) {
        kshell_puts(ctx, "command illegal\n");
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
    char *cmdline, ps1[32], ps2[32];
    unsigned int count;
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
            kshell_fmtenv(&nctx, ps1, sizeof(ps1), "PS1");
            kshell_fmtenv(&nctx, ps2, sizeof(ps2), "PS2");

            cmdline = readline(rstate, ps1, ps2);
            if (!rstate->len)
                continue;

            errno = -ENOERR;
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
    list_for_each_entry(cmd, &kshell_list, list) {
        if (unlikely(!cmd->name))
            break;
        kshell_printf(ctx, "\t%-16s - %s\n", cmd->name, cmd->desc);
    }
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
