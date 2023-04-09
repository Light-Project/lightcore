/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <cmdline.h>
#include <export.h>

void kshell_fmtenv(struct kshell_context *ctx, char *buff, size_t size, const char *name)
{
    escape_string(buff, size, kshell_getenv(ctx, name) ?: "");
}
EXPORT_SYMBOL(kshell_fmtenv);
