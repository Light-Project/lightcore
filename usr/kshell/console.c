/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <ascii.h>
#include <sched.h>
#include <export.h>

unsigned int kshell_read(struct kshell_context *ctx, char *str, unsigned int len)
{
    if (ctx->read)
        return ctx->read(str, len, ctx->data);
    return UINT_MAX;
}
EXPORT_SYMBOL(kshell_read);

void kshell_write(struct kshell_context *ctx, const char *str, unsigned int len)
{
    if (ctx->write)
        ctx->write(str, len, ctx->data);
}
EXPORT_SYMBOL(kshell_write);

int kshell_vscanf(struct kshell_context *ctx, const char *fmt, va_list args)
{
    struct readline_state *rstate = ctx->readline;
    const char *prompt;
    char *strbuf;

    rstate = readline_alloc(rstate->read, rstate->write, rstate->data);
    if (!rstate)
        return 0;

    prompt = kshell_getenv(ctx, "PS3");
    do strbuf = readline(rstate, prompt, prompt);
    while (!rstate->len);

    readline_free(rstate);
    return vsscanf(strbuf, fmt, args);
}
EXPORT_SYMBOL(kshell_vscanf);

int kshell_scanf(struct kshell_context *ctx, const char *fmt, ...)
{
    va_list args;
    int len;

    va_start(args, fmt);
    len = kshell_vscanf(ctx, fmt, args);
    va_end(args);

    return len;
}
EXPORT_SYMBOL(kshell_scanf);

int kshell_vprintf(struct kshell_context *ctx, const char *fmt, va_list args)
{
    char strbuf[256];
    int len;

    len = vscnprintf(strbuf, sizeof(strbuf), fmt, args);
    kshell_write(ctx, strbuf, len);

    return len;
}
EXPORT_SYMBOL(kshell_vprintf);

int kshell_printf(struct kshell_context *ctx, const char *fmt, ...)
{
    va_list args;
    int len;

    va_start(args, fmt);
    len = kshell_vprintf(ctx, fmt, args);
    va_end(args);

    return len;
}
EXPORT_SYMBOL(kshell_printf);

bool kshell_ctrlc(struct kshell_context *ctx)
{
    unsigned int len;
    char buff[8];

    if (unlikely(ctx->canceled))
        return true;

    len = kshell_read(ctx, buff, sizeof(buff));
    while (len--) {
        if (buff[len] == ASCII_ETX) {
            ctx->canceled = true;
            return true;
        }
    }

    return false;
}
EXPORT_SYMBOL(kshell_ctrlc);
