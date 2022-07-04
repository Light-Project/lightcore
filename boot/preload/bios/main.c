/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <linkage.h>
#include <kernel.h>
#include <crypto/crc32-table.h>
#include <lightcore/asm/byteorder.h>
#include <asm-generic/header.h>

static uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    crc ^= 0xffffffff;
    while (len--)
        crc = crc32_byte(crc, bigreal_readb(src++));
    return crc ^ 0xffffffff;
}

static __noreturn void biosdisk_boot(void)
{
    struct uboot_head *head = skzalloc(512);
    uint32_t size, load, entry;
    uint32_t rawload, blkcount;
    uint32_t oldcrc, newcrc;

    biosdisk_read(boot_dev, head, load_seek, 1);
    if (be32_to_cpu(head->magic) != UBOOT_MAGIC)
        panic("biosdisk bad image\n");

    size = be32_to_cpu(head->size);
    load = be32_to_cpu(head->load);
    entry = be32_to_cpu(head->ep);
    oldcrc = be32_to_cpu(head->dcrc);

    rawload = load - sizeof(*head);
    blkcount = DIV_ROUND_UP(size + sizeof(*head), 512);

    pr_boot("data size: %#08x\n", size);
    pr_boot("load address: %#08x\n", load);
    pr_boot("entry point: %#08x\n", entry);
    biosdisk_read(boot_dev, (void *)rawload, load_seek, blkcount);

    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc)
        panic("crc error 0x%x->0x%x\n", oldcrc, newcrc);

    pr_boot("start kboot...\n");
    kboot_start((entry & ~0xffff) >> 4, entry & 0xffff);
}

asmlinkage void __noreturn main(void)
{
    video_clear();
    pr_init(video_print);

    a20_enable();
    biosdisk_boot();
}
