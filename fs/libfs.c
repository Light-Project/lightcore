/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <filesystem.h>
#include <kmalloc.h>
#include <export.h>

static struct kcache *inode_cache;
static struct kcache *superblock_cache;

struct inode *fslib_inode_alloc(void)
{
    return kcache_zalloc(inode_cache, GFP_KERNEL);
}
EXPORT_SYMBOL(fslib_inode_alloc);

void fslib_inode_free(struct inode *inode)
{
    kcache_free(inode_cache, inode);
}
EXPORT_SYMBOL(fslib_inode_free);

struct superblock *fslib_superblock_alloc(void)
{
    struct superblock *sb;

    sb = kcache_zalloc(superblock_cache, GFP_KERNEL);

    return sb;
}
EXPORT_SYMBOL(fslib_superblock_alloc);

void fslib_superblock_free(struct superblock *sb)
{
    kcache_free(superblock_cache, sb);
}
EXPORT_SYMBOL(fslib_superblock_free);

void __init fslib_init(void)
{
    inode_cache = kcache_create(
        "libfs-inode", sizeof(struct inode),
        KCACHE_PANIC
    );
    superblock_cache = kcache_create(
        "libfs-superblock", sizeof(struct superblock),
        KCACHE_PANIC
    );
}
