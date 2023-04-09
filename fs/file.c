/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <filesystem.h>
#include <syscall.h>

ssize_t sys_read(unsigned int fd, char *buf, size_t len)
{
    if(!fd || !buf || !len)
        return -EINVAL;

    return 0;
}

ssize_t sys_write(unsigned int fd, const char *buf, size_t len)
{

    return 0;
}

int sys_open(const char *filename, int flags, umode_t mode)
{

    return 0;
}

int sys_close(unsigned int fd)
{

    return 0;
}

long sys_sync(void)
{

    return 0;
}
