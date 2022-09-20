/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <linkage.h>
#include <kernel.h>
#include <asm/header.h>
#include <lightcore/asm/byteorder.h>

#ifndef CONFIG_PRELOAD_EXTENDED
# include <crypto/crc32-table.h>

static uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t loop = crc;
    while (len--)
        loop = crc32_byte(loop, bigreal_readb(src++));
    return loop ^ crc;
}

static __noreturn void biosdisk_boot(void)
{
    struct uboot_head *head = zalloca(512);
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

    pr_boot("data size: %#x\n", size);
    pr_boot("load address: %#x\n", load);
    pr_boot("entry point: %#x\n", entry);

    if (size > memory_top - entry)
        panic("image too big %#x->%#x\n", size, memory_top - entry);

    biosdisk_read(boot_dev, (void *)rawload, load_seek, blkcount);
    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc)
        panic("crc error %#x->%#x\n", oldcrc, newcrc);

    pr_boot("start kboot...\n");
    kboot_start((entry & ~0xffff) >> 4, entry & 0xffff);
}
#else /* CONFIG_PRELOAD_EXTENDED */
static __noreturn void biosdisk_boot(void)
{
    struct kboot_params *head = zalloca(1024);
    uint32_t hsect, ksize, kload;

    biosdisk_read(boot_dev, head, load_seek, 2);
    head = (void *)head + 0x1f1;
    if (memcmp(&head->header, "HdrS", 4))
        panic("biosdisk bad image\n");

    hsect = head->setup_sects + 1;
    ksize = head->syssize;
    kload = head->code32_start;

    pr_boot("header size: %#x\n", hsect);
    pr_boot("data size: %#x\n", ksize);
    pr_boot("load address: %#x\n", kload);

    ksize = DIV_ROUND_UP(ksize, 512);
    biosdisk_read(boot_dev, (void *)0x10000, load_seek, hsect);
    biosdisk_read(boot_dev, (void *)kload, load_seek + hsect, ksize);

    pr_boot("start kboot...\n");
    kboot_start(0x1000, 0x200);
}
#endif /* CONFIG_PRELOAD_EXTENDED */

asmlinkage void __noreturn main(void)
{
#ifdef CONFIG_PRELOAD_CON_VGA
    video_clear();
    pr_init(video_print);
#endif
#ifdef CONFIG_PRELOAD_CON_SER
    serial_init();
    pr_init(serial_print);
#endif

    a20_enable();
    pr_boot("boot device: %d\n", boot_dev);
    biosdisk_boot();
}
