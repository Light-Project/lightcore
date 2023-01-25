/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <string.h>
#include <sections.h>
#include <export.h>

void kfree_const(const char *block)
{
    if (!in_rodata_section(block))
        kfree(block);
}
EXPORT_SYMBOL(kfree_const);

char *kstrdup(const char *s, gfp_t gfp)
{
    char *block;

    if (!s)
        return NULL;

    block = kmalloc(strlen(s) + 1, gfp);
    if (block)
        strcpy(block, s);

    return block;
}
EXPORT_SYMBOL(kstrdup);

const char *kstrdup_const(const char *s, gfp_t gfp)
{
    if (in_rodata_section(s))
        return s;

    return kstrdup(s, gfp);
}
EXPORT_SYMBOL(kstrdup_const);

char *kstrndup(const char *s, size_t max, gfp_t gfp)
{
    size_t len;
    char *block;

    if (!s)
        return NULL;

    len = strnlen(s, max);
    block = kmalloc(len + 1, gfp);

    if (block) {
        strncpy(block, s, len);
        block[len] = '\0';
    }

    return block;
}
EXPORT_SYMBOL(kstrndup);
