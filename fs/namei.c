/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <fs.h>

static state path_walk(const char *path, struct nameidata *namei)
{

    while (*path++ == '/')
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

struct file *namei_open()
{

}
