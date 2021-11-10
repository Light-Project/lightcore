/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/clocksource/gx6605s.h>
#include <asm/proc.h>
#include <asm/io.h>

uint32_t time_read(void)
{
    return readl(TIM_BASE + GX6605S_TIM_VALUE);
}

void mdelay(uint32_t ms)
{
    uint32_t oldt;

    oldt = readl(TIM_BASE + GX6605S_TIM_VALUE);
    while (readl(TIM_BASE + GX6605S_TIM_VALUE) < (oldt + ms))
        cpu_relax();
}

void timer_init(uint32_t freq)
{
    writel(TIM_BASE + GX6605S_TIM_CLKDIV, 0);
    writel(TIM_BASE + GX6605S_TIM_RELOAD, 0);
    writel(TIM_BASE + GX6605S_TIM_VALUE, 0);

    writel(TIM_BASE + GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    writel(TIM_BASE + GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_EN);
    writel(TIM_BASE + GX6605S_TIM_CLKDIV, (freq / 1000) - 1);
    writel(TIM_BASE + GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}
