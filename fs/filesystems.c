/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <fs.h>
#include <string.h>
#include <export.h>

LIST_HEAD(filesystem_list);

struct filesystem_type *filesystem_find(const char *name)
{
    struct filesystem_type *fs = NULL;

    list_for_each_entry(fs, &filesystem_list, list)
        if (!strcmp(name, fs->name))
            return fs;

    return NULL;
}
EXPORT_SYMBOL(filesystem_find);

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

void filesystem_unregister(struct filesystem_type *fs)
{
    list_del(&fs->list);
}
EXPORT_SYMBOL(filesystem_unregister);
