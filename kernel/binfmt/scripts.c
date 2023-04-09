/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <initcall.h>
#include <binfmt.h>


static struct binfmt_type binfmt_elf_type = {
    .name = "elf",
    .load_bin = elf_loadbin,
};

static state binfmt_elf_init(void)
{
    return binfmt_register(&binfmt_elf_type);
}
fs_initcall(binfmt_elf_init);
