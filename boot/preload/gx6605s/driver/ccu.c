/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/platform/gx6605s.h>
#include <asm/io.h>

static uint32_t pll_get_factors(uint32_t freq, uint32_t *n, uint32_t *m)
{
    uint32_t div, pll;

    if (freq > 1152 * MHZ)
        div = 2;
    else if (freq > 864 * MHZ)
        div = 3;
    else
        div = 4;

    pll = freq / (osc_freq / div);

    *n = div;
    *m = pll;

    return osc_freq / div * pll;
}

static uint32_t dto_freq_set(unsigned int channel, uint32_t fin, uint32_t freq)
{
    uint32_t div = (uint64_t)freq * 0x40000000 / fin;
    void *reg = GCTL_BASE + GX6605S_DTO1_CONFIG + (4 * (channel - 1));

    /**
     * div = freq * 2^30 / fin
     * freq = fin *  div / 2^30
     */

    writel(reg, (1 << 31));
    writel(reg, (0 << 31));
    writel(reg, (1 << 31));
    writel(reg, (1 << 31)| div);
    writel(reg, (1 << 31)|(1 << 30)| div);
    writel(reg, (1 << 31)|(1 << 30)| div);
    writel(reg, (1 << 31)| div);

    return (uint64_t)fin * div / 0x40000000;
}

/**
 * ccu_cpu - Set the freq of cpu
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_cpu(uint32_t dto, uint32_t freq)
{
    return dto_freq_set(12, dto, freq);
}

/**
 * ccu_axi - Set the freq of axi
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_axi(uint32_t cpu, uint32_t freq)
{
    uint32_t val, div = cpu / freq;

    val = readl(GCTL_BASE + GX6605S_CLOCK_DIV_CONFIG2);
    val &= ~(0x0f << 0);
    val |=  (div & 0x0f) << 0;
    writel(GCTL_BASE + GX6605S_CLOCK_DIV_CONFIG2, val);

    return cpu / div;
}

/**
 * ccu_ahb - Set the freq of the ahb
 * @freq: freq want to set
 * @return: set success freq
 * TODO: computational problems
 */
uint32_t ccu_ahb(uint32_t cpu, uint32_t freq)
{
    uint32_t val, div = cpu / freq;

    val = readl(GCTL_BASE + GX6605S_SOURCE_SEL2);
    val &= ~(0x0f << 28);
    val |= ((div + 2) & 0x0f) << 28;
    writel(GCTL_BASE + GX6605S_SOURCE_SEL2, val);

    /* Selcect the CPU clock to from xtal to PLL */
    val = readl(GCTL_BASE + GX6605S_SOURCE_SEL1);
    val |= GX6605S_SOURCE_SEL1_CPU;
    writel(GCTL_BASE + GX6605S_SOURCE_SEL1, val);

    return cpu / div;
}

/**
 * ccu_apb - Set the freq of the apb
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_apb(uint32_t dto, uint32_t freq)
{
    uint32_t val, ret;

    ret = dto_freq_set(10, dto, freq); /* APB ir and other*/
    val = readl(GCTL_BASE + GX6605S_SOURCE_SEL1);
    val |= GX6605S_SOURCE_SEL1_IR;
    writel(GCTL_BASE + GX6605S_SOURCE_SEL1, val);

    dto_freq_set(11, dto, freq); /* APB uart */
    val = readl(GCTL_BASE + GX6605S_SOURCE_SEL1);
    val |= GX6605S_SOURCE_SEL1_UART;
    writel(GCTL_BASE + GX6605S_SOURCE_SEL1, val);

    return ret;
}

/**
 * ccu_dram - Set the freq of the dram
 * @freq: freq want to set
 * @return: set success freq
 */
uint32_t ccu_dram(uint32_t freq)
{
    uint32_t val, n, m, ret;

    ret = pll_get_factors(freq, &n, &m);
    writel(GCTL_BASE + GX6605S_PLL_DDR_BASE, (1 << 14)|(n << 8)| m);
    val = readl(GCTL_BASE + GX6605S_PLL_DDR_BASE);
    val &= ~BIT(14);
    writel(GCTL_BASE + GX6605S_PLL_DDR_BASE, val);

    /* this is for dramc, phy and port clock */
    val = readl(GCTL_BASE + GX6605S_SOURCE_SEL1);
    val |= GX6605S_SOURCE_SEL1_DRAMC;
    writel(GCTL_BASE + GX6605S_SOURCE_SEL1, val);

    return ret;
}

uint32_t ccu_init(void)
{
    uint32_t val, n, m, ret;

    writel(GCTL_BASE + GX6605S_SOURCE_SEL1, 0x00);

    ret = pll_get_factors(dto_freq, &n, &m);
    writel(GCTL_BASE + GX6605S_PLL_DTO_BASE, (1 << 14)|(n << 8)| m);
    val = readl(GCTL_BASE + GX6605S_PLL_DTO_BASE);
    val &= ~BIT(14);
    writel(GCTL_BASE + GX6605S_PLL_DTO_BASE, val);

    pll_get_factors(dvb_freq, &n, &m);
    writel(GCTL_BASE + GX6605S_PLL_DVB_BASE, (1 << 14)|(n << 8)| m);
    val = readl(GCTL_BASE + GX6605S_PLL_DVB_BASE);
    val &= ~BIT(14);
    writel(GCTL_BASE + GX6605S_PLL_DVB_BASE, val);

    return ret;
}
