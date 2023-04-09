/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <filesystem.h>


static state ramfs_create()
{

}


static struct filesystem_type ramfs_type = {
    .name = "ramfs",

};

static state ramfs_register()
{
    return filesystem_register(&ramfs_type);
}
fs_initcall(ramfs_register);
