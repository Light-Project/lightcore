/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <fs/dirent.h>

static struct dirent *dirent_alloc(struct superblock *sb )
{
    struct dirent *dirent;

    dirent = kzalloc(sizeof(dirent), GFP_KERNEL);
    if (!dirent)
        return NULL;


    /* fill dirent node */
    dirent->sb = sb;

    list_head_init(&dirent->child);


    return dirent;
}