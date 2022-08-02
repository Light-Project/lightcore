/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "VFL"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <filesystem.h>
#include <kmalloc.h>
#include <task.h>
#include <printk.h>
#include <export.h>

state kernel_link(const char *oldname, const char *newname)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_link);

state kernel_symlink(const char *oldname, const char *newname)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_symlink);

state kernel_unlink(const char *filename)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_unlink);

state kernel_chdir(const char *pathname)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_chdir);

state kernel_chroot(const char *pathname)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_chroot);

state kernel_mount(const char *devn, const char *fsn, enum mount_flags flags)
{

    return -ENOERR;
}
EXPORT_SYMBOL(kernel_mount);

state kernel_unmount(const char *pathname)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_unmount);

state kernel_execve(const char *file, const char *const *argv, const char *const *envp)
{
    return -ENOERR;
}
EXPORT_SYMBOL(kernel_execve);
