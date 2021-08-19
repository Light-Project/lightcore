/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "fatfs: " fmt

#include <mm.h>
#include <init/initcall.h>
#include <fs.h>

#include "ff.h"
#include "diskio.h"

DSTATUS disk_initialize ( BYTE pdrv)
{
    return STA_NOINIT;
}

DWORD get_fattime(void) {
    return 0;
}

static state fatfs_file_open(struct inode *inode, struct file *file)
{
    FIL *ffile;

    ffile = kzalloc(sizeof(*ffile), GFP_KERNEL);
    if (!ffile)
        return -ENOMEM;
    file->data = ffile;

    return -ENOERR;
}

static state fatfs_file_read(struct file *file, void *dest, size_t len, loff_t *off)
{
    return -ENOERR;
}

static state fatfs_file_write(struct file *file, void *src, size_t len, loff_t *off)
{

    return -ENOERR;
}

static struct file_ops fatfs_file_ops = {
    .open = fatfs_file_open,
    .read = fatfs_file_read,
    .write = fatfs_file_write,
};

static struct

static struct fs_type fatfs_type = {
    .name = "fatfs",
};

static state fatfs_init(void)
{
    return filesystem_register(&fatfs_type);
}
fs_initcall(fatfs_init);
