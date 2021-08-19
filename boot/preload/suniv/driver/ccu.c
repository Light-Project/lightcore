/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/clk/suniv.h>

#include <asm/io.h>
#include <asm/proc.h>

void ccu_cpu(uint32_t freq)
{
    uint32_t k, m, val;

    if (freq > 1152000000) {
        k = 2;
        m = 1;
    } else if (freq > 768000000) {
        k = 3;
        m = 2;
    } else {
        k = 1;
        m = 1;
    }

    writel(CCU_BASE + SUNIV_CPU_CLK_SRC, 0x01 << 16);

    val  = (0x1 << 31);
    val |= (((freq / (24000000 * k / m)) - 1) & 0x1f) << 8;
    val |= ((k - 1) & 0x3) << 4;
    val |= (m - 1) & 0x3;
    writel(CCU_BASE + SUNIV_PLL_CPU_CTRL, val);

    while(!((readl(CCU_BASE + SUNIV_PLL_CPU_CTRL) >> 28) & 0x01))
        cpu_relax();

    writel(CCU_BASE + SUNIV_CPU_CLK_SRC, (2<<16) | (1<<8) | (2<<0));
}

void ccu_sys(uint32_t freq)
{
    uint32_t k, m, val;

    if (freq > 1152000000) {
        k = 2;
        m = 1;
    } else if (freq > 768000000) {
        k = 3;
        m = 2;
    } else {
        k = 1;
        m = 1;
    }

    writel(CCU_BASE + SUNIV_AHB_APB_HCLKC_CFG, (1 << 16)|(1 << 12)|(1 << 4));

    val  = 1 << 31;
    val |= 1 << 18;
    val |= (((freq / (24000000 * k / m)) -1) & 0x1f) << 8;
    val |= ((k - 1) & 0x03) << 4;
    val |= (m - 1) & 0x03;
    writel(CCU_BASE + SUNIV_PLL_PERIPH_CTRL, val);

    while(!((readl(CCU_BASE + SUNIV_PLL_PERIPH_CTRL) >> 28) & 0x01))
        cpu_relax();

    writel(CCU_BASE + SUNIV_AHB_APB_HCLKC_CFG, 3 << 12);
}
