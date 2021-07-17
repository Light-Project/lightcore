/* SPDX-License-Identifier: GPL-2.0-or-later */
// #include <system/syscall.h>
#include <fs.h>
#include <string.h>
#include <export.h>

static LIST_HEAD(filesystem_list);

struct fs_type *filesystem_find(char *name)
{
    struct fs_type *fs = NULL;

    list_for_each_entry(fs, &filesystem_list, list)
        if(strcmp(name, fs->name))
            return fs;
    
    return NULL;
}
EXPORT_SYMBOL(filesystem_find);

state filesystem_register(struct fs_type *fs)
{
    struct fs_type *ffs;
    
    ffs = filesystem_find(fs->name);
    if(ffs)
        return -EINVAL;

    list_add(&filesystem_list, &fs->list);

    return -ENOERR;
}
EXPORT_SYMBOL(filesystem_register);

state filesystem_unregister(struct fs_type *fs)
{
    list_del(&fs->list);
    return -ENOERR;
}
EXPORT_SYMBOL(filesystem_unregister);

long sys_sysfs(int ops, unsigned long arg1, unsigned long arg2)
{
    state ret = -EINVAL;

    return ret;
}
