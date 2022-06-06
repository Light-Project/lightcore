/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "minix.h"
#include <initcall.h>
#include <kmalloc.h>
#include <printk.h>

static struct kcache *min_info_cache;

state minix_btree_read(struct inode *inode, loff_t pos, void *buff, size_t len)
{
    struct minix_sb_info *msb_info = sb_to_minix(inode->sb);

    if (msb_info->version == MINIX_V1)
        return minix_btree_v1_read(inode, pos, buff, len);
    else
        return minix_btree_v2_read(inode, pos, buff, len);
}

state minix_btree_write(struct inode *inode, loff_t pos, const void *buff, size_t len)
{
    struct minix_sb_info *msb_info = sb_to_minix(inode->sb);

    if (msb_info->version == MINIX_V1)
        return minix_btree_v1_write(inode, pos, buff, len);
    else
        return minix_btree_v2_write(inode, pos, buff, len);
}

static struct inode *minix_inode_alloc(struct superblock *sb)
{
    struct minix_inode_info *min_info;
    min_info = kcache_zalloc(min_info_cache, GFP_KERNEL);
    return min_info ? &min_info->inode : NULL;
}

static void minix_inode_free(struct inode *inode)
{
    struct minix_inode_info *min_info;
    min_info = inode_to_minix(inode);
    kcache_free(min_info_cache, min_info);
}

static state minix_inode_v1_getinfo(struct inode *inode)
{
    struct superblock *sb = inode->sb;
    struct minix_inode_info *mio_info = inode_to_minix(inode);
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    struct minix_inode minode;
    unsigned int count;
    loff_t offset;
    state ret;

    offset = (2 + msb_info->imap_blocks + msb_info->zmap_blocks) *
              BLOCK_SIZE + inode->inum * sizeof(minode);

    ret = sb_read(sb, offset, &minode, sizeof(minode));
    if (unlikely(ret))
        return ret;

    inode->mode = minode.mode;
    inode->uid = minode.uid;
    inode->gid = minode.gid;
    inode->nlink = minode.nlinks;
    inode->size = minode.size;
    inode->mtime.tv_sec = minode.time;

    for (count = 0; count < 9; ++count)
        mio_info->data_v1[count] = minode.zone[count];

    return -ENOERR;
}

static state minix_inode_v2_getinfo(struct inode *inode)
{
    struct superblock *sb = inode->sb;
    struct minix_inode_info *mio_info = inode_to_minix(inode);
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    struct minix2_inode minode;
    unsigned int count;
    loff_t offset;
    state ret;

    offset = (2 + msb_info->imap_blocks + msb_info->zmap_blocks) *
              BLOCK_SIZE + inode->inum * sizeof(minode);

    ret = sb_read(sb, offset, &minode, sizeof(minode));
    if (unlikely(ret))
        return ret;

    inode->mode = minode.mode;
    inode->uid = minode.uid;
    inode->gid = minode.gid;
    inode->nlink = minode.nlinks;
    inode->size = minode.size;
    inode->mtime.tv_sec = minode.mtime;
    inode->atime.tv_sec = minode.atime;
    inode->ctime.tv_sec = minode.ctime;

    for (count = 0; count < 10; ++count)
        mio_info->data_v2[count] = minode.zone[count];

    return -ENOERR;
}

state minix_inode_getinfo(struct superblock *sb, unsigned long inum)
{
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    struct inode *inode;

    inode = inode_alloc(sb);
    if (unlikely(!inode))
        return -ENOMEM;

    inode->inum = inum;

    if (msb_info->version == MINIX_V1)
        return minix_inode_v1_getinfo(inode);
    else
        return minix_inode_v2_getinfo(inode);
}

static state minix_inode_v1_update(struct inode *inode)
{
    struct superblock *sb = inode->sb;
    struct minix_inode_info *mio_info = inode_to_minix(inode);
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    struct minix_inode minode;
    unsigned int count;
    loff_t offset;
    state ret;

    offset = (2 + msb_info->imap_blocks + msb_info->zmap_blocks) *
              BLOCK_SIZE + inode->inum * sizeof(minode);

    ret = sb_read(sb, offset, &minode, sizeof(minode));
    if (unlikely(ret))
        return ret;

    minode.mode = inode->mode;
    minode.uid = inode->uid;
    minode.gid = inode->gid;
    minode.nlinks = inode->nlink;
    minode.size = inode->size;
    minode.time = inode->mtime.tv_sec;

    if (S_ISCHR(inode->mode) || S_ISBLK(inode->mode))
        minode.zone[0] = 0;
    else for (count = 0; count < 9; ++count)
        minode.zone[count] = mio_info->data_v1[count];

    return sb_write(sb, offset, &minode, sizeof(minode));
}

static state minix_inode_v2_update(struct inode *inode)
{
    struct superblock *sb = inode->sb;
    struct minix_inode_info *mio_info = inode_to_minix(inode);
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    struct minix2_inode minode;
    unsigned int count;
    loff_t offset;
    state ret;

    offset = (2 + msb_info->imap_blocks + msb_info->zmap_blocks) *
              sb->block_size + inode->inum * sizeof(minode);

    ret = sb_read(sb, offset, &minode, sizeof(minode));
    if (unlikely(ret))
        return ret;

    minode.mode = inode->mode;
    minode.uid = inode->uid;
    minode.gid = inode->gid;
    minode.nlinks = inode->nlink;
    minode.size = inode->size;
    minode.mtime = inode->mtime.tv_sec;
    minode.atime = inode->atime.tv_sec;
    minode.ctime = inode->ctime.tv_sec;

    if (S_ISCHR(inode->mode) || S_ISBLK(inode->mode))
        minode.zone[0] = 0;
    else for (count = 0; count < 10; ++count)
        minode.zone[count] = mio_info->data_v2[count];

    return sb_write(sb, offset, &minode, sizeof(minode));
}

static state minix_inode_update(struct inode *inode)
{
    struct minix_sb_info *msb_info = sb_to_minix(inode->sb);

    if (msb_info->version == MINIX_V1)
        return minix_inode_v1_update(inode);
    else
        return minix_inode_v2_update(inode);
}

static struct superblock_ops minix_superblock_ops = {
    .inode_alloc = minix_inode_alloc,
    .inode_free = minix_inode_free,
    .inode_update = minix_inode_update,
};

static struct superblock *minix_mount(struct fsdev *fsdev, enum mount_flags flags)
{
    struct minix_superblock *msb;
    struct minix3_superblock *m3sb;
    struct minix_sb_info *minfo;
    unsigned long imap_size, zmap_size;
    void *block;
    state ret;

    minfo = kzalloc(sizeof(*minfo) + sizeof(*msb), GFP_KERNEL);
    msb = (void *)minfo + sizeof(*minfo);
    m3sb = (void *)minfo + sizeof(*minfo);
    if (!minfo)
        return ERR_PTR(-ENOMEM);

    ret = fsdev_read(fsdev, BLOCK_SIZE, msb, 64);
    if (ret < 0)
        goto error_minfo;

    minfo->sb.fsdev = fsdev;
    minfo->sb.maxsize = msb->max_size;
    minfo->sb.ops = &minix_superblock_ops;

    minfo->sb_buffer = msb;
    minfo->ninodes = msb->ninodes;
    minfo->nzones = msb->nzones;
    minfo->imap_blocks = msb->imap_blocks;
    minfo->zmap_blocks = msb->zmap_blocks;
    minfo->firstdatazone = msb->firstdatazone;
    minfo->log_zone_size = msb->log_zone_size;
    minfo->state = msb->state;

    if (msb->magic == MINIX_SUPER_MAGIC) {
        minfo->version = MINIX_V1;
        minfo->dirsize = 16;
        minfo->namelen = 14;
    } else if (msb->magic == MINIX_SUPER_MAGIC2) {
        minfo->version = MINIX_V1;
        minfo->dirsize = 32;
        minfo->namelen = 30;
    } else if (msb->magic == MINIX2_SUPER_MAGIC) {
        minfo->version = MINIX_V2;
        minfo->dirsize = 16;
        minfo->namelen = 14;
        minfo->nzones = msb->zones;
    } else if (msb->magic == MINIX2_SUPER_MAGIC2) {
        minfo->version = MINIX_V2;
        minfo->dirsize = 32;
        minfo->namelen = 30;
        minfo->nzones = msb->zones;
    } else if (m3sb->magic == MINIX3_SUPER_MAGIC) {
        minfo->version = MINIX_V3;
        minfo->dirsize = 64;
        minfo->namelen = 60;
        minfo->nzones = m3sb->zones;
        minfo->imap_blocks = m3sb->imap_blocks;
        minfo->zmap_blocks = m3sb->zmap_blocks;
        minfo->firstdatazone = m3sb->firstdatazone;
        minfo->log_zone_size = m3sb->log_zone_size;
        minfo->sb.maxsize = m3sb->max_size;
    } else {
        pr_err("illegal minix magic (V1 | V2 | V3)\n");
        goto error_minfo;
    }

    if (!minfo->imap_blocks || !minfo->zmap_blocks ||
        (minfo->version == MINIX_V1 && minfo->sb.maxsize >
        (7 + 512 + 512*512) * BLOCK_SIZE)) {
        pr_err("bad superblock\n");
        goto error_minfo;
    }

    imap_size = minfo->imap_blocks * minfo->sb.block_size;
    zmap_size = minfo->zmap_blocks * minfo->sb.block_size;

    block = kzalloc(imap_size * zmap_size, GFP_KERNEL);
    if (!block) {
        pr_err("failed to allocate map\n");
        goto error_minfo;
    }

    minfo->imap_buffer = block;
    minfo->zmap_buffer = block + minfo->imap_blocks * minfo->sb.block_size;
    minfo->imap_pos = 2 + minfo->imap_blocks * minfo->sb.block_size;
    minfo->zmap_pos = minfo->imap_pos + minfo->zmap_blocks * minfo->sb.block_size;
    minfo->imap_size = imap_size;
    minfo->zmap_size = zmap_size;

    ret = fsdev_read(fsdev, minfo->imap_pos, minfo->imap_buffer, imap_size);
    if (unlikely(ret)) {
        pr_err("unable to read inode bitmaps\n");
        goto error_minfo;
    }

    ret = fsdev_read(fsdev, minfo->zmap_pos, minfo->zmap_buffer, zmap_size);
    if (unlikely(ret)) {
        pr_err("unable to read data block bitmaps\n");
        goto error_minfo;
    }

    bit_set_le(minfo->imap_buffer, 0);
    bit_set_le(minfo->zmap_buffer, 0);

    imap_size = DIV_ROUND_UP(minfo->ninodes, minfo->sb.block_size * BITS_PER_BYTE);
    if (minfo->imap_blocks < imap_size) {
        pr_err("insufficient imap blocks\n");
        goto error_minfo;
    }

    zmap_size = DIV_ROUND_UP(minfo->ninodes, minfo->sb.block_size * BITS_PER_BYTE);
    if (minfo->zmap_blocks < zmap_size) {
        pr_err("insufficient zmap blocks\n");
        goto error_minfo;
    }

    if (flags & MOUNT_WRONLY &&
        minfo->version == MINIX_V3) {
        minfo->state &= ~MINIX_VALID_FS;
        fsdev_write(fsdev, BLOCK_SIZE, msb, 64);
    }

    if (!(minfo->state & MINIX_VALID_FS))
        pr_warn("mounting unchecked filesystem\n");

    if (minfo->state & MINIX_ERROR_FS)
        pr_warn("mounting filesystem with errors\n");

    return &minfo->sb;

error_minfo:
    kfree(minfo);
    return ERR_PTR(ret);
}

static struct filesystem_type minix_type = {
    .name = FSTYPE_NAME,
    .mount = minix_mount,
};

static state minix_init(void)
{
    min_info_cache = KCACHE_CREATE(struct minix_inode_info, 0);
    if (!min_info_cache)
        return -ENOMEM;
    return filesystem_register(&minix_type);
}
fs_initcall(minix_init);
