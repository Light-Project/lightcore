/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <filesystem.h>
#include <string.h>
#include <export.h>

static LIST_HEAD(filesystem_list);

/**
 * filesystem_find - find a filesystem
 * @name: filesystem name to look for
 */
struct filesystem_type *filesystem_find(const char *name)
{
    struct filesystem_type *fs = NULL;

    list_for_each_entry(fs, &filesystem_list, list)
        if (!strcmp(name, fs->name))
            return fs;

    return NULL;
}
EXPORT_SYMBOL(filesystem_find);

/**
 * filesystem_register - register a filesystem
 * @fs: filesystem to register
 */
state filesystem_register(struct filesystem_type *fs)
{
    struct filesystem_type *already;

    already = filesystem_find(fs->name);
    if (already)
        return -EINVAL;

    list_add(&filesystem_list, &fs->list);
    return -ENOERR;
}
EXPORT_SYMBOL(filesystem_register);

/**
 * filesystem_unregister - unregister a filesystem
 * @fs: filesystem to unregister
 */
void filesystem_unregister(struct filesystem_type *fs)
{
    list_del(&fs->list);
}
EXPORT_SYMBOL(filesystem_unregister);
