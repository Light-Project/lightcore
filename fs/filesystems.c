/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <filesystem.h>
#include <string.h>
#include <export.h>

LIST_HEAD(filesystem_list);
SPIN_LOCK(filesystem_lock);

/**
 * filesystem_find - find a filesystem
 * @name: filesystem name to look for
 */
struct filesystem_type *filesystem_find(const char *name)
{
    struct filesystem_type *fs = NULL;

    spin_lock(&filesystem_lock);
    list_for_each_entry(fs, &filesystem_list, list)
        if (!strcmp(name, fs->name))
            return fs;
    spin_unlock(&filesystem_lock);

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

    spin_lock(&filesystem_lock);
    list_add(&filesystem_list, &fs->list);
    spin_unlock(&filesystem_lock);
    return -ENOERR;
}
EXPORT_SYMBOL(filesystem_register);

/**
 * filesystem_unregister - unregister a filesystem
 * @fs: filesystem to unregister
 */
void filesystem_unregister(struct filesystem_type *fs)
{
    spin_lock(&filesystem_lock);
    list_del(&fs->list);
    spin_unlock(&filesystem_lock);
}
EXPORT_SYMBOL(filesystem_unregister);
