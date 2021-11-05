/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <linkage.h>
#include <crc_table.h>
#include <driver/platform/gx6605s.h>
#include <lightcore/asm/byteorder.h>
#include <asm-generic/header.h>
#include <asm/mmu.h>

static inline uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;
    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];
    return tmp ^ crc;
}

static inline void spiflash_boot(void)
{
    struct uboot_head head;
    uint32_t size, load, entry, oldcrc, newcrc;

    spi_init();

    if (norflash_id()) {
        pr_boot("norflash not found\n");
        return;
    }

    norflash_read((void *)&head, head_addr, sizeof(head));
    if (be32_to_cpu(head.magic) != UBOOT_MAGIC) {
        pr_boot("norflash bad image\n");
        return;
    }

    size = be32_to_cpu(head.size);
    load = be32_to_cpu(head.load);
    entry = be32_to_cpu(head.ep);
    oldcrc = be32_to_cpu(head.dcrc);

    pr_boot("data size: %d\n", size);
    pr_boot("load address: %#x\n", load);
    pr_boot("entry point: %#x\n", entry);
    norflash_read((void *)load, head_addr + sizeof(head), size);

    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc) {
        pr_boot("crc error %#x->0x%x\n", oldcrc, newcrc);
        return;
    }

    pr_boot("boot form norflash...\n");
    kboot_start((void *)entry);
}

asmlinkage void exception_handle(uint32_t vector)
{
    panic("exception %d\n", vector);
}

asmlinkage void main(void)
{
    uint32_t clock_dto, clock_cpu;
    uint32_t clock_axi, clock_ahb;
    uint32_t clock_apb, clock_dram;

    clock_dto = ccu_init();
    clock_cpu = ccu_cpu(clock_dto, cpu_freq);
    clock_axi = ccu_axi(clock_cpu, axi_freq);
    clock_ahb = ccu_ahb(clock_cpu, ahb_freq);
    clock_apb = ccu_apb(clock_dto, apb_freq);
    clock_dram = ccu_dram(dram_freq);

    uart_init(clock_apb, clock_uart);
    pr_init(uart_print);

    pr_boot("system clock setting:\n");
    pr_boot("    dto clk:  %dMhz\n", clock_dto / MHZ);
    pr_boot("    cpu clk:  %dMhz\n", clock_cpu / MHZ);
    pr_boot("    axi clk:  %dMhz\n", clock_axi / MHZ);
    pr_boot("    ahb clk:  %dMhz\n", clock_ahb / MHZ);
    pr_boot("    apb clk:  %dMhz\n", clock_apb / MHZ);
    pr_boot("    dram clk: %dMhz\n", clock_dram / MHZ);

    dramc_init();
    spiflash_boot();

    panic("no bootable image\n");
}
