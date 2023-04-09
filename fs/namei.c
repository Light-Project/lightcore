/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <kmalloc.h>
#include <filesystem.h>

struct namei {
    struct

};

static state path_walk(const char *path, struct namei *namei)
{

    while (*path == '/')
        ++path;

    if (!*path)
        return -ENOERR;

    for (;;) {

        if (*path == '.') {

        }

        while (*++path == '/')
        if (!*path)
            return -ENOERR;

    }
}


state namei_kernel_path(const char *path)
{
    state ret;



    return ret;
}
