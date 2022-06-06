/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "minix.h"
#include <bitops.h>
#include <timekeeping.h>
#include <printk.h>

static state minix_imap_update(struct minix_sb_info *msb_info, unsigned int bit)
{
    struct superblock *sb = &msb_info->sb;
    loff_t offset;
    void *buffer;

    offset = msb_info->imap_pos + bit / BYTES_PER_LONG;
    buffer = msb_info->imap_buffer + bit / BYTES_PER_LONG;

    return sb_write(sb, offset, buffer, BYTES_PER_LONG);
}

static state minix_zmap_update(struct minix_sb_info *msb_info, unsigned int bit)
{
    struct superblock *sb = &msb_info->sb;
    loff_t offset;
    void *buffer;

    offset = msb_info->zmap_pos + bit / BYTES_PER_LONG;
    buffer = msb_info->zmap_buffer + bit / BYTES_PER_LONG;

    return sb_write(sb, offset, buffer, BYTES_PER_LONG);
}

static state minix_inode_clear(struct inode *inode)
{
    struct superblock *sb = inode->sb;
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    loff_t offset;
    void *zeroed;

    offset = (2 + msb_info->imap_blocks + msb_info->zmap_blocks) * sb->block_size;

    if (msb_info->version == MINIX_V1) {
        zeroed = skzalloc(sizeof(struct minix_inode));
        offset += inode->inum * sizeof(struct minix_inode);
        return sb_write(inode->sb, offset, zeroed, sizeof(struct minix_inode));
    } else {
        zeroed = skzalloc(sizeof(struct minix2_inode));
        offset += inode->inum * sizeof(struct minix2_inode);
        return sb_write(inode->sb, offset, zeroed, sizeof(struct minix2_inode));
    }
}

struct inode *minix_bitmap_inode_alloc(struct inode *parent)
{
    struct minix_sb_info *msb_info = sb_to_minix(parent->sb);
    unsigned int find, imap_bits = msb_info->imap_size * BITS_PER_BYTE;
    struct inode *inode;

    inode = inode_alloc(parent->sb);
    if (unlikely(!inode))
        return ERR_PTR(-ENOMEM);

    spin_lock(&msb_info->lock);
    find = find_first_zero_le(msb_info->zmap_buffer, imap_bits);
    if (unlikely(find >= imap_bits)) {
        spin_unlock(&msb_info->lock);
        return ERR_PTR(-ENOSPC);
    }
    bit_set_le(msb_info->zmap_buffer, find);
    spin_unlock(&msb_info->lock);
    minix_imap_update(msb_info, find);

    inode->inum = find;
    inode->mtime = timekeeping_get_realtime_ts();
    inode->atime = inode->ctime = inode->mtime;

    return inode;
}

void minix_bitmap_inode_free(struct inode *inode)
{
    struct minix_sb_info *msb_info = sb_to_minix(inode->sb);
    unsigned int inum, shift = inode->sb->block_bits + 3;

    inum = inode->inum;
    if (inum < 1 || inum > msb_info->ninodes) {
        pr_err("illegal inode_free: beyond datazone\n");
        return;
    }

    if (inum >> shift >= msb_info->imap_blocks) {
        pr_err("illegal inode_free: nonexistent imap\n");
        return;
    }

    minix_inode_clear(inode);
    spin_lock(&msb_info->lock);
    if (bit_test_clr_le(msb_info->imap_buffer, inum))
        pr_err("illegal inode_free: bit already cleared\n");
    spin_unlock(&msb_info->lock);
    minix_imap_update(msb_info, inum);
}

unsigned int minix_bitmap_block_alloc(struct superblock *sb)
{
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    unsigned int find, zmap_bits = msb_info->zmap_size * BITS_PER_BYTE;

    spin_lock(&msb_info->lock);
    find = find_first_zero_le(msb_info->zmap_buffer, zmap_bits);
    if (likely(find < zmap_bits)) {
        bit_set_le(msb_info->zmap_buffer, find);
        spin_unlock(&msb_info->lock);
        minix_zmap_update(msb_info, find);
        find += msb_info->firstdatazone - 1;
        if (find < msb_info->firstdatazone && find >= msb_info->nzones)
            return 0;
        return find;
    }
    spin_unlock(&msb_info->lock);

    return 0;
}

void minix_bitmap_block_free(struct superblock *sb, unsigned long sector)
{
    struct minix_sb_info *msb_info = sb_to_minix(sb);
    unsigned int clr_bit = sector - msb_info->firstdatazone + 1;

    if (sector < msb_info->firstdatazone || sector >= msb_info->nzones) {
        pr_err("illegal block_free: beyond datazone\n");
        return;
    }

    spin_lock(&msb_info->lock);
    if (bit_test_clr_le(msb_info->zmap_buffer, clr_bit))
        pr_err("illegal block_free: bit already cleared\n");
    spin_unlock(&msb_info->lock);
    minix_zmap_update(msb_info, clr_bit);
}
