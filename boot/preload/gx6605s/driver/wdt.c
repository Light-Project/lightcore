/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/watchdog/gx6605s.h>
#include <asm/io.h>

static void wdt_init(uint32_t clk)
{
    uint32_t val;

    val = readl(WDT_BASE + GX6605S_WDT_MATCH);
    val &= ~GX6605S_WDT_MATCH_CLKDIV;
    val |= (osc_freq / clk - 1) << 16;
    writel(WDT_BASE + GX6605S_WDT_MATCH, val);
}

static void wdt_timeout_set(uint32_t ms)
{
    uint32_t val;

    val = readl(WDT_BASE + GX6605S_WDT_MATCH);
    val &= ~GX6605S_WDT_MATCH_TIMEOUT;
    val |= (0x10000 - ms) << 0;
    writel(WDT_BASE + GX6605S_WDT_MATCH, val);
}

static void wdt_start(void)
{
    uint32_t val;

    val = readl(WDT_BASE + GX6605S_WDT_CTRL);
    val |= GX6605S_WDT_CTRL_RESET | GX6605S_WDT_CTRL_ENABLE;
    writel(WDT_BASE + GX6605S_WDT_CTRL, val);
}

static void wdt_stop(void)
{
    uint32_t val;

    writel(WDT_BASE + GX6605S_WDT_WSR, 0);
    val = readl(WDT_BASE + GX6605S_WDT_CTRL);
    val &= ~(GX6605S_WDT_CTRL_RESET | GX6605S_WDT_CTRL_ENABLE);
    writel(WDT_BASE + GX6605S_WDT_CTRL, val);
}

void __noreturn panic_reboot(void)
{
    wdt_stop();
    wdt_init(1000);
    wdt_timeout_set(10);
    wdt_start();

    /* Wait wdt reboot */
    for (;;)
    halt();
}
