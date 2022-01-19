/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <filesystem.h>
#include <syscall.h>

state kernel_execve(const char *file, const char *const *argv, const char *const *envp)
{
    return -ENOERR;
}
