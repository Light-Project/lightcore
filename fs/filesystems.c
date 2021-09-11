/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <fs.h>
#include <string.h>
#include <export.h>

LIST_HEAD(filesystem_list);

struct fs_type *filesystem_find(const char *name)
{
    struct fs_type *fs = NULL;

    list_for_each_entry(fs, &filesystem_list, list)
        if (strcmp(name, fs->name))
            return fs;

    return NULL;
}
EXPORT_SYMBOL(filesystem_find);

state filesystem_register(struct fs_type *fs)
{
    struct fs_type *ffs;

    ffs = filesystem_find(fs->name);
    if (ffs)
        return -EINVAL;

    list_add(&filesystem_list, &fs->list);

    return -ENOERR;
}
EXPORT_SYMBOL(filesystem_register);

void filesystem_unregister(struct fs_type *fs)
{
    list_del(&fs->list);
}
EXPORT_SYMBOL(filesystem_unregister);
