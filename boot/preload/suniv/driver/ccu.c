/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <driver/clk/suniv.h>

#include <asm/io.h>
#include <asm/proc.h>

uint32_t pll_get_factors(uint32_t freq, uint32_t *k, uint32_t *m, uint32_t *n)
{
    uint32_t mul, div, pll;

    if (freq > 1024 * MHZ) {
        mul = 4;
        div = 2;
    } else if (freq > 768 * MHZ) {
        mul = 4;
        div = 3;
    } else if (freq >= osc_freq) {
        mul = 4;
        div = 4;
    } else {
        mul = 1;
        div = 4;
    }

    pll = freq / (osc_freq * mul / div);

    *k = mul - 1;
    *m = div - 1;
    *n = pll - 1;

    return (osc_freq * pll * mul) / div;
}

/**
 * ccu_periph - Set the freq of the periph
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_periph(uint32_t freq)
{
    uint32_t n, k = 1;

    if (freq > 768 * MHZ)
        k = 2;

    n = freq / (osc_freq * k);

    writel(CCU_BASE + SUNIV_AHB_APB_HCLKC_CFG, 1 << 12);

    /* freq = osc_freq * n * k */
    writel(CCU_BASE + SUNIV_PLL_PERIPH_CTRL, (1 << 31)|((n - 1) << 8)|((k - 1) << 4));
    while(!((readl(CCU_BASE + SUNIV_PLL_PERIPH_CTRL) >> 28) & 0x01))
        cpu_relax();

    return osc_freq * n * k;
}

/**
 * ccu_cpu - Set the freq of the cpu
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_cpu(uint32_t freq)
{
    uint32_t k, m, n, ret;

    ret = pll_get_factors(freq, &k, &m, &n);

    writel(CCU_BASE + SUNIV_CPU_CLK_SRC, 0x01 << 16);
    writel(CCU_BASE + SUNIV_PLL_CPU_CTRL, (1 << 31)|(n << 8)|(k << 4)| m);
    while(!((readl(CCU_BASE + SUNIV_PLL_CPU_CTRL) >> 28) & 0x01))
        cpu_relax();

    writel(CCU_BASE + SUNIV_CPU_CLK_SRC, 2 << 16);
    return ret;
}

/**
 * ccu_dram - Set the freq of the dram
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_dram(uint32_t freq)
{
    uint32_t k, m, n, ret;

    ret = pll_get_factors(freq, &k, &m, &n);
    writel(CCU_BASE + SUNIV_PLL_DDR_CTRL, (1 << 31)|(n << 8)|(k << 4)| m);
    while(!((readl(CCU_BASE + SUNIV_PLL_DDR_CTRL) >> 28) & 0x01))
        cpu_relax();

    return ret;
}

/**
 * ccu_sys - Set the freq of the system bus
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_sys(uint32_t freq)
{
    uint32_t n = clock_periph / freq;

    n = clamp(n, 1, 4);

    /*
     * AHB_CLK_SRC = PLL_PERIPH / n (AHB_PRE_DIV)
     * AHB_CLK = AHB_CLK_SRC / 1 (AHB_CLK_RATIO)
     * APB_CLK = AHB_CLK / 2 (APB_CLK_RATIO)
     */
    writel(CCU_BASE + SUNIV_AHB_APB_HCLKC_CFG, (3 << 12)|((n - 1) << 6));

    return clock_periph / n;
}
