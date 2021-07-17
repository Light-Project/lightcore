/* SPDX-License-Identifier: GPL-2.0-or-later */

#define pr_fmt(fmt) "cpio: " fmt

#include <init/initcall.h>
#include <fs.h>
#include <fs/cpio.h>


static struct fs_type cpio_type = {
    .name = "cpio"
};

static state cpio_init(void)
{
    return filesystem_register(&cpio_type);
}

fs_initcall(cpio_init);
