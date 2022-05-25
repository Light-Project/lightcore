/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <ascii.h>
#include <export.h>

unsigned int kshell_read(struct kshell_context *ctx, char *str, unsigned int len)
{
    return ctx->read(str, len, ctx->data);
}
EXPORT_SYMBOL(kshell_read);

void kshell_write(struct kshell_context *ctx, const char *str, unsigned int len)
{
    ctx->write(str, len, ctx->data);
}
EXPORT_SYMBOL(kshell_write);

int kshell_vprintf(struct kshell_context *ctx, const char *str, va_list args)
{
    char strbuf[256];
    int len;

    len = vsnprintf(strbuf, sizeof(strbuf), str, args);
    ctx->write(strbuf, len, ctx->data);

    return len;
}
EXPORT_SYMBOL(kshell_vprintf);

int kshell_printf(struct kshell_context *ctx, const char *str, ...)
{
    va_list args;
    int len;

    va_start(args,str);
    len = kshell_vprintf(ctx, str, args);
    va_end(args);

    return len;
}
EXPORT_SYMBOL(kshell_printf);

bool kshell_ctrlc(struct kshell_context *ctx)
{
    unsigned int len;
    char buff[32];

    len = kshell_read(ctx, buff, sizeof(buff));
    while (len--) {
        if (buff[len] == ASCII_ETX)
            return true;
    }

    return false;
}
EXPORT_SYMBOL(kshell_ctrlc);
