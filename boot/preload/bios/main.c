/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <asm/io.h>

#define load_seek   63
#define load_addr   ((void *)0x10000)

extern void *_ld_dts_start;

void main(void)
{
    uint32_t val;

    video_clear();
    pr_init(video_print);

    pr_boot("Boot device ID = 0x%02x\n", BOOT_DEV);
    biosdisk_read(BOOT_DEV, load_addr, load_seek, 1);

    /* Judge whether the image exists through magic */
    val = ext_readw(load_addr + 0x1fe);
    if(val != 0xaa55)
        panic("can't find Image: 0x%02x\n", val);

    /* Read the number of blocks in the image */
    val = ext_readl(load_addr + 0x1f4);
    pr_boot("Image size: %d blocks\n", val);
    biosdisk_read(BOOT_DEV, load_addr, load_seek, val);

    pr_boot("start kboot...\n");
    kboot_start(((uint32_t)load_addr) >> 4);
}
