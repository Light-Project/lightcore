/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "MINIX: " fmt

#include <kmalloc.h>
#include <fs/minix.h>

static state minix_mount(struct fsdev *fsdev, enum mount_flag flags)
{
    struct minix_superblock *msb = (uint8_t [64]){};
    struct minix3_superblock *m3sb = msb;
    struct minix_info *minfo;
    state ret;

    minfo = kzalloc(sizeof(*minfo), GFP_KERNEL);
    if (!minfo)
        return -ENOMEM;

    ret = fsdev_read(fsdev, 0, msb, 64);
    if (ret < 0)
        goto error_minfo;

    minfo->s_ninodes = msb->ninodes;
    minfo->s_nzones = msb->nzones;
    minfo->s_imap_blocks = msb->imap_blocks;
    minfo->s_zmap_blocks = msb->zmap_blocks;
    minfo->s_firstdatazone = msb->firstdatazone;
    minfo->s_log_zone_size = msb->log_zone_size;
    minfo->s_state = msb->state;

    if (msb->magic == MINIX_SUPER_MAGIC) {
        minfo->s_version = MINIX_V1;
        minfo->s_dirsize = 16;
        minfo->s_namelen = 14;
    } else if (msb->magic == MINIX_SUPER_MAGIC2) {
        minfo->s_version = MINIX_V1;
        minfo->s_dirsize = 32;
        minfo->s_namelen = 30;
    } else if (msb->magic == MINIX2_SUPER_MAGIC) {
        minfo->s_version = MINIX_V2;
        minfo->s_dirsize = 16;
        minfo->s_namelen = 14;
        minfo->s_nzones = msb->zones;
    } else if (msb->magic == MINIX2_SUPER_MAGIC2) {
        minfo->s_version = MINIX_V2;
        minfo->s_dirsize = 32;
        minfo->s_namelen = 30;
        minfo->s_nzones = msb->zones;
    } else if (msb->magic == MINIX3_SUPER_MAGIC) {
        minfo->s_version = MINIX_V3;
        minfo->s_dirsize = 64;
        minfo->s_namelen = 60;
        minfo->s_nzones = m3sb->zones;
        minfo->s_imap_blocks = m3sb->imap_blocks;
        minfo->s_zmap_blocks = m3sb->zmap_blocks;
        minfo->s_firstdatazone = m3sb->firstdatazone;
        minfo->s_log_zone_size = m3sb->log_zone_size;
    } else {
        goto error_minfo;
    }



error_minfo:
    kfree(minfo);
    return ret;
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
