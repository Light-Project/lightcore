/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "cpio"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <filesystem.h>
#include <fs/cpio.h>

static state cpio_read(struct file *file, void *dest, size_t len, loff_t *off)
{

    return -ENOERR;
}

struct file_ops cpio_file_ops = {
    .read = cpio_read,
};

static struct dirent *cpio_lookup(struct inode *dir, struct dirent *dirent, unsigned int flags)
{

    return -ENOERR;
}

struct inode_ops cpio_inode_ops = {
    .lookup = cpio_lookup,
};

static struct superblock_ops cpio_sb_ops = {
    .inode_alloc = fslib_inode_alloc,
    .inode_free = fslib_inode_free,
};

static struct superblock *cpio_mount(struct fsdev *fsdev, enum mount_flags flags)
{
    struct cpio_inode csb;
    struct superblock *sb;
    state ret;

    /* read the image superblock and check it */
    ret = fsdev_read(fsdev, 0, &csb, sizeof(csb));
    if (ret < 0)
        return ERR_PTR(ret);

    if (memcmp(csb.magic, CPIO_MAGIC, 6))
        return ERR_PTR(-EINVAL);

    /* fill super block */
    sb = kzalloc(sizeof(*sb), GFP_KERNEL);
    if (!sb)
        return ERR_PTR(-ENOMEM);

    sb->ops = &cpio_sb_ops;

    return sb;
}

static struct filesystem_type cpio_type = {
    .name = "cpio",
    .mount = cpio_mount,
};

static state cpio_init(void)
{
    return filesystem_register(&cpio_type);
}
fs_initcall(cpio_init);
