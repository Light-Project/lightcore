/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <crc_table.h>
#include <lightcore/asm/byteorder.h>
#include <asm-generic/header.h>

#define head_addr   0x8000

static uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;
    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];
    return tmp ^ crc;
}

static bool check_fel(void)
{
    return !strncmp((char *)0x8, ".FEL", 4);
}

static void spiflash_boot(void)
{
    struct uboot_head head;
    uint32_t size, load, entry, oldcrc, newcrc;

    spi_init(clock_spi);

    if (norflash_id()) {
        pr_boot("Read NorFlash error!\n");
        return;
    }

    norflash_read((void *)&head, head_addr, sizeof(head));
    if (be32_to_cpu(head.magic) != UBOOT_MAGIC)
        return;

    size = be32_to_cpu(head.size);
    load = be32_to_cpu(head.load);
    entry = be32_to_cpu(head.ep);
    oldcrc = be32_to_cpu(head.dcrc);

    pr_boot("Data Size: %d\n", size);
    pr_boot("Load Address: 0x%x\n", load);
    pr_boot("Entry Point: 0x%x\n", entry);
    norflash_read((void *)load, head_addr + sizeof(head), size);

    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc) {
        pr_boot("crc error 0x%x->0x%x\n", oldcrc, newcrc);
        return;
    }

    pr_boot("boot form NorFlash\n");
    kboot_start((void *)entry);
}

static void sdcard_boot(void)
{
    struct uboot_head head;
    uint32_t size, load, entry, oldcrc, newcrc;

    return;

    mmc_init(clock_mmc);

    size = be32_to_cpu(head.size);
    load = be32_to_cpu(head.load);
    entry = be32_to_cpu(head.ep);
    oldcrc = be32_to_cpu(head.dcrc);

    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc) {
        pr_boot("crc error 0x%x->0x%x\n", oldcrc, newcrc);
        return;
    }

    pr_boot("boot form Sdcard\n");
    kboot_start((void *)entry);
}

void main(void)
{
    ccu_sys(clock_sys);
    ccu_cpu(clock_cpu);

    uart_init(clock_uart);
    pr_init(uart_print);

    pr_boot("CPU Speed: %dMhz\n", clock_cpu / 1000000);
    pr_boot("BUS Speed: %dMhz\n", clock_sys / 1000000);
    pr_boot("DDR Speed: %dMhz\n", clock_dram / 1000000);

    dram_init(clock_dram);

    if (check_fel())
        goto fail;

    spiflash_boot();
    sdcard_boot();

fail:
    pr_boot("return FEL\n");
    return;
}
