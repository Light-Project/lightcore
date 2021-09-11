/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "cpio: " fmt

#include <mm.h>
#include <initcall.h>
#include <fs.h>
#include <fs/cpio.h>

static state cpio_read(struct file *file, void *dest, size_t len, loff_t *off)
{

    return -ENOERR;
}

struct file_ops cpio_file_ops = {
    .read = cpio_read,
};

static struct dcache *cpio_inode_lookup(struct inode *inode,
                                        struct dcache *dcache, unsigned int flags)
{

    return -ENOERR;
}

struct inode_ops cpio_inode_ops = {
    .readdir = cpio_inode_lookup,
};

static state cpio_mount(struct fs_type *fs, struct fsdev *dev,
                        enum mount_flag flag, struct super_block **sbp)
{
    struct super_block *sb;

    sb = kmalloc(sizeof(*sb), GFP_KERNEL);
    if (!sb)
        return -ENOERR;

    return -ENOERR;
}

static struct fs_type cpio_type = {
    .name = "cpio",
    .mount = cpio_mount,
};

static state cpio_init(void)
{
    return filesystem_register(&cpio_type);
}
fs_initcall(cpio_init);
