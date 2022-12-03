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

const char *kvasprintf_const(gfp_t flags, const char *fmt, va_list args)
{
    if (!strchr(fmt, '%'))
        return kstrdup_const(fmt, flags);

    if (!strcmp(fmt, "%s"))
        return kstrdup_const(va_arg(args, const char *), flags);

    return kvasprintf(flags, fmt, args);
}
EXPORT_SYMBOL(kvasprintf_const);

char *kasprintf(gfp_t flags, const char *fmt, ...)
{
    va_list args;
    char *block;

    va_start(args, fmt);
    block = kvasprintf(flags, fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(kasprintf);

const char *kasprintf_const(gfp_t flags, const char *fmt, ...)
{
    va_list args;
    const char *block;

    va_start(args, fmt);
    block = kvasprintf_const(flags, fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(kasprintf_const);

char *vasprintf(const char *fmt, va_list args)
{
    return kvasprintf(GFP_KERNEL, fmt, args);
}
EXPORT_SYMBOL(vasprintf);

const char *vasprintf_const(const char *fmt, va_list args)
{
    return kvasprintf_const(GFP_KERNEL, fmt, args);
}
EXPORT_SYMBOL(vasprintf_const);

char *asprintf(const char *fmt, ...)
{
    va_list args;
    char *block;

    va_start(args, fmt);
    block = vasprintf(fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(asprintf);

const char *asprintf_const(const char *fmt, ...)
{
    va_list args;
    const char *block;

    va_start(args, fmt);
    block = vasprintf_const(fmt, args);
    va_end(args);

    return block;
}
EXPORT_SYMBOL(asprintf_const);
