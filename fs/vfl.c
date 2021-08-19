/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <fs.h>
#include <kernel/syscall.h>

ssize_t vfl_read(struct file *file, void *buf, size_t len, loff_t *pos)
{
    if(!(file->fmode & FMODE_RDONLY))
        return -EBADF;

    if(!file->ops->read)
        return -EINVAL;

    return file->ops->read(file, buf, len, pos);
}

ssize_t vfl_write(struct file *file, void *buf, size_t len, loff_t *pos)
{
    if(!(file->fmode & FMODE_WRONLY))
        return -EBADF;

    if(!file->ops->read)
        return -EINVAL;

    return file->ops->write(file, buf, len, pos);
}

struct file *vfl_open(const char *name, int flags, umode_t mode)
{
    struct file *file;

    file = kzalloc(sizeof(*file), GFP_KERNEL);
    if (!file)
        return NULL;

    return file;
}