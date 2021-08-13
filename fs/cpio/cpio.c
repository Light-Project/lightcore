/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "cpio: " fmt

#include <init/initcall.h>
#include <fs.h>
#include <fs/cpio.h>

// static state cpio_read(struct file *fp, void *dest, size_t len, loff_t *off)
// {


// }

// static struct file_ops cpio_file_ops = {
//     .read = cpio_read,
// };

// static state cpio_mount()
// {

// }

static struct fs_type cpio_type = {
    .name = "cpio"
};

static state cpio_init(void)
{
    return filesystem_register(&cpio_type);
}
fs_initcall(cpio_init);
