/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "romfs"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <filesystem.h>
#include <fs/romfs.h>

static inline uint32_t romfs_chechsum(const void *start, size_t size)
{
    const be32 *data = start;
    uint32_t sum;

    for (sum = 0; size; --size)
        sum += be32_to_cpup(data);

    return sum;
}

static struct file_ops romfs_file_ops = {

};

static struct inode *romfs_iget(struct superblock *sb, unsigned long pos)
{
    struct romfs_inode *rnode;
    unsigned long offset, len;
    struct inode *inode;
    state ret;

    rnode = kzalloc(ROMFH_SIZE + ROMFS_MAXFN, GFP_KERNEL);
    if (!rnode)
        return ERR_PTR(-ENOMEM);

    for (;;) {
        ret = sb_read(sb, offset, rnode, ROMFH_SIZE);
        if (ret)
            goto free;

        /* its not a hard link */
        if ((rnode->next & ROMFH_TYPE) != ROMFH_HRD)
            break;

        /* link entry */
        offset = rnode->spec & ROMFH_MASK;
    }

    ret = sb_read(sb, offset + ROMFH_SIZE, rnode->name, ROMFS_MAXFN);
    if (ret)
        goto free;

    len = strnlen(rnode->name, ROMFS_MAXFN);

    switch (rnode->next & ROMFH_TYPE) {
        case ROMFH_REG:
            inode->fops = &romfs_file_ops;
            break;
        case ROMFH_DIR:

            break;
        case ROMFH_SYM:

            break;
        default:

    }

free:
    kfree(rnode);
    return ERR_PTR(ret);
}

static struct dirent *romfs_lookup(struct inode *dir, struct dirent *dirent, unsigned int flags)
{
    struct romfs_inode *rnode;
    unsigned long offset, size;
    state ret;

    rnode = kzalloc(ROMFH_SIZE + dirent->len, GFP_KERNEL);
    if (!rnode)
        return ERR_PTR(-ENOMEM);

    /* Get romfs inode. */
    ret = sb_read(dir->sb, offset, &rnode, ROMFH_SIZE);
    if (ret < 0)
        goto free;

    while (offset && offset < size) {
        ret = sb_read(dir->sb, offset, &rnode, ROMFH_SIZE + dirent->len);
        if (ret < 0)
            goto free;

        if (!strncmp(dirent->name, rnode->name, dirent->len)) {
            break;
        }

        /* next entry */
        offset = be32_to_cpu(rnode->next) & ROMFH_MASK;
    }

    return ;
free:
    kfree(rnode);
    return ERR_PTR(ret);
}

static struct superblock_ops romfs_sb_ops = {
};

static struct superblock *romfs_mount(struct fsdev *fsdev, enum mount_flags flags)
{
    struct romfs_superblock rsb;
    struct superblock *sb;
    unsigned long pos, size;
    struct inode *root;
    size_t len;
    state ret;

    /* read the image superblock and check it */
    ret = fsdev_read(fsdev, 0, &rsb, sizeof(rsb));
    if (ret < 0)
        goto error_rsb;

    if (memcmp(rsb.magic, ROMFS_MAGIC, 8)) {
        ret = -ENOMEM;
        goto error_rsb;
    }

    /* fill super block */
    sb = kzalloc(sizeof(*sb), GFP_KERNEL);
    if (!sb) {
        ret = -ENOMEM;
        goto error_rsb;
    }

    sb->ops = &romfs_sb_ops;

    pos = (ROMFH_SIZE + len + 1 + ROMFH_PAD) & ROMFH_MASK;

    len = strnlen(rsb, );

    /* find the root directory */
    pos = (ROMFH_SIZE + len + 1 + ROMFH_PAD) & ROMFH_MASK;

    root = romfs_iget(sb, pos);
    if (IS_INVAL(root))
        return PTR_INVAL(root);

    sb->root = root;

    return sb;
error_sb:
    kfree(sb);
error_rsb:
    kfree(rsb);
    return ERR_PTR(ret);
}

static struct filesystem_type romfs_type = {
    .name = "romfs",
    .mount = romfs_mount,
};

static state romfs_init(void)
{
    return filesystem_register(&romfs_type);
}
fs_initcall(romfs_init);
