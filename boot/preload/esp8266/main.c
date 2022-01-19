/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <asm-generic/header.h>
#include <crc-table.h>
#include <asm/io.h>

static const char *reset_reason[] = {
    "no mean",
    "power on reset",
    "external reset",
    "software reset",
    "watch-dog reset",
    "deep-sleep reset",
    "slc reset",
};

static uint32_t crc32(const uint32_t *s, int len, uint32_t crc)
{
    uint32_t buf, tmp = crc;
    unsigned int count;

    len /= 4;

    while (len--) {
        buf = readl(s++);
        for (count = 0; count < 4; ++count) {
            crc = crc32_table[(crc ^ (buf & 0xff)) & 0xff] ^ (crc >> 8);
            buf >>= 8;
        }
    }

    return tmp ^ crc;
}

static void kernel_check(void *addr)
{
    struct boot_head *boot_head = addr;
    uint32_t size, newcrc, oldcrc;
    uint8_t cmp[17];

    pr_boot("check addr: 0x%x\n", addr);

    for (uint8_t c = 0; c <= 4; c++)
        cmp[c * 4] = readl(&boot_head->magic[c*4]);
    cmp[17] = '\0';

    if (strcmp((char *)&cmp, "lightcore!"))
        panic("can't find kernel!\n");

    size = readl(&boot_head->size);
    pr_boot("kernel size: 0x%d\n", size);

    oldcrc = readl(&boot_head->crc);
    newcrc = crc32((void *)(boot_head + 1), size, ~0);

    if (oldcrc != newcrc)
        panic("crc error 0x%x->0x%x\n", oldcrc, newcrc);
}

static void cpu_pll(char mul2)
{
    uint32_t val;

    val = readl(CCU_BASE);
    val &= (mul2 | 0xfffffffe);
    val |= (mul2 & 0x01);
    writel(CCU_BASE, val);

    pr_boot("system clock setting:\n");
    pr_boot("  cpu clk: %dMhz\n", APB_FREQ * (mul2 + 1) / MHZ);
    pr_boot("  bus clk: %dMhz\n", APB_FREQ / MHZ);
    pr_boot("  osc clk: %dMhz\n", OSC_FREQ / MHZ);
}

asmlinkage void main(void)
{
    uint8_t reason;

    uart_init();
    pr_init(uart_print);

    cpu_pll(1);
    timer_init();

    reason = readb((void *)(0x60000700 + 0x14));
    reason = min(reason, ARRAY_SIZE(reset_reason));
    pr_boot("reset: %s\n", reset_reason[reason]);

    /* XIP can only map 1MiB space at the same time ! */

    /* arg0: Map address                     */
    /* 0: 0x000000 - 0x0fffff \              */
    /*                         \             */
    /*                           0x40200000  */
    /*                         /             */
    /* 1: 0x100000 - 0x1fffff /              */

    /* arg1: Flash offset                    */
    /* 0: 0x000000 - 0x1fffff                */
    /* 1: 0x200000 - 0x3fffff                */
    /* 2: 0x400000 - 0x5fffff                */
    /* 3: 0x600000 - 0x7fffff                */

    /* arg2: Xip cache size                  */
    /* 0: 16KiB                              */
    /* 1: 32KiB                              */

    Cache_Read_Disable();
    Cache_Read_Enable(0, 0, 1);
    pr_boot("mmap flash to memory\n");

    kernel_check((void *)CONFIG_XIP_BASE);

    pr_boot("start kernel...\n");
    pr_boot("total boot time: %dms\n", time_read());

    timer_stop();
    kboot_start((void *)CONFIG_XIP_BASE);
}
