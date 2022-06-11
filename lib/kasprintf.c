/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <string.h>
#include <kmalloc.h>
#include <panic.h>
#include <export.h>

char *kvasprintf(gfp_t flags, const char *fmt, va_list args)
{
    unsigned int size1, size2;
    va_list cargs;
    char *block;

    va_copy(cargs, args);
    size1 = vsnprintf(NULL, 0, fmt, args);
    va_end(cargs);

    block = kmalloc(size1 + 1, flags);
    if (unlikely(!block))
        return NULL;

    size2 = vsnprintf(block, size1 + 1, fmt, args);
    BUG_ON(size1 != size2);

    return block;
}
EXPORT_SYMBOL(kvasprintf);

const char *kvasprintf_const(gfp_t gfp, const char *fmt, va_list args)
{
    if (!strchr(fmt, '%'))
        return kstrdup_const(fmt, gfp);

    if (!strcmp(fmt, "%s"))
        return kstrdup_const(va_arg(args, const char *), gfp);

    return kvasprintf(gfp, fmt, args);
}
EXPORT_SYMBOL(kvasprintf_const);

char *kasprintf(gfp_t gfp, const char *fmt, ...)
{
    va_list args;
    char *block;

    va_start(args, fmt);
    block = kvasprintf(gfp, fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(kasprintf);

const char *kasprintf_const(gfp_t gfp, const char *fmt, ...)
{
    va_list args;
    const char *block;

    va_start(args, fmt);
    block = kvasprintf_const(gfp, fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(kasprintf_const);
