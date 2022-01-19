/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <crc-table.h>
#include <driver/irqchip/suniv.h>
#include <lightcore/asm/byteorder.h>
#include <asm-generic/header.h>
#include <driver/gpio/sunxi.h>
#include <asm/regs.h>
#include <asm/io.h>

uint32_t clock_cpu, clock_periph;
uint32_t clock_ahb, clock_dram;

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

    spi_init(clock_spi);

    if (norflash_id()) {
        pr_boot("norflash not found\n");
        return;
    }

    norflash_read((void *)&head, head_addr, sizeof(head));
    if (be32_to_cpu(head.magic) != UBOOT_MAGIC) {
        pr_boot("Norflash bad image\n");
        return;
    }

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

    pr_boot("Boot form norflash...\n");
    kboot_start((void *)entry);
}

#ifdef CONFIG_PRELOAD_MMC

static void sdcard_boot(void)
{
    uint8_t buff[512];
    struct uboot_head *head = (void *)buff;
    uint32_t size, load, entry, oldcrc, newcrc;

    mmc_init(clock_mmc);

    if (sdcard_init()) {
        pr_boot("Sdcard not found\n");
        return;
    }

    sdcard_read((void *)buff, sdcard_lba, 1);

    if (be32_to_cpu(head->magic) != UBOOT_MAGIC) {
        pr_boot("Sdcard bad image\n");
        return;
    }

    size = be32_to_cpu(head->size);
    load = be32_to_cpu(head->load);
    entry = be32_to_cpu(head->ep);
    oldcrc = be32_to_cpu(head->dcrc);

    pr_boot("Data Size: %d\n", size);
    pr_boot("Load Address: 0x%x\n", load);
    pr_boot("Entry Point: 0x%x\n", entry);
    sdcard_read((void *)load - sizeof(*head),
                sdcard_lba, DIV_ROUND_CLOSEST(size, 512));

    newcrc = crc32((void *)load, size, ~0);
    if (oldcrc != newcrc) {
        pr_boot("crc error 0x%x->0x%x\n", oldcrc, newcrc);
        return;
    }

    pr_boot("Boot form Sdcard...\n");
    kboot_start((void *)entry);
}

#endif /* CONFIG_PRELOAD_MMC */

static inline void set_jtag(void)
{
    writel(PIO_F + SUNXI_GPIO_CFG0, 0x303033);
}

static inline bool check_fel(void)
{
    return strncmp((char *)0x8, ".BT0", 4);
}

static inline void recovery_fel(void)
{
    uint32_t val;

    val = cp15_get(c1, c0, 0);
    cp15_set(c1, c0, 0, val | (1 << 13));

    kboot_start(FEL_ENTRY);
}

void main(void)
{
    clock_periph = ccu_periph(periph_freq);
    clock_ahb = ccu_sys(sys_freq);
    clock_cpu = ccu_cpu(cpu_freq);
    clock_dram = ccu_dram(dram_freq);

    uart_init(clock_uart);
    pr_init(uart_print);

    pr_boot("system clock setting:\n");
    pr_boot("    cpu clk:    %dMhz\n", clock_cpu / MHZ);
    pr_boot("    dram clk:   %dMhz\n", clock_dram / MHZ);
    pr_boot("    periph clk: %dMhz\n", clock_periph / MHZ);
    pr_boot("    ahb clk:    %dMhz\n", clock_ahb / MHZ);
    pr_boot("    apb clk:    %dMhz\n", clock_apb / MHZ);

    dramc_init(dram_freq);

    if (check_fel()) {
        pr_boot("Return FEL\n");
        set_jtag();
        return;
    }

    spiflash_boot();
    spi_deinit();

#ifdef CONFIG_PRELOAD_MMC
    sdcard_boot();
    mmc_deinit();
#endif

    set_jtag();
    pr_boot("Recovery FEL\n");
    uart_sync();

    recovery_fel();
}
