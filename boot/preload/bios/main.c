/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <linkage.h>
#include <crc_table.h>
#include <lightcore/asm/byteorder.h>
#include <asm-generic/header.h>

static inline uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;
    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];
    return tmp ^ crc;
}

static inline __noreturn void biosdisk_boot(void)
{
    struct uboot_head *head = (void *)(uint8_t [512]){};
    uint32_t size, load, entry;

    biosdisk_read(boot_dev, head, load_seek, 512 >> 9);
    if (be32_to_cpu(head->magic) != UBOOT_MAGIC)
        panic("biosdisk bad image\n");

    size = be32_to_cpu(head->size);
    load = be32_to_cpu(head->load);
    entry = be32_to_cpu(head->ep);

    pr_boot("data size: %d\n", size);
    pr_boot("load address: %#x\n", load);
    pr_boot("entry point: %#x\n", entry);
    biosdisk_read(boot_dev, (void *)load - sizeof(*head), load_seek, size >> 9);

    pr_boot("start kboot...\n");
    kboot_start((entry & ~0xffff) >> 4, entry & 0xffff);
}

asmlinkage void __noreturn main(void)
{
    video_clear();
    pr_init(video_print);
    biosdisk_boot();
}
