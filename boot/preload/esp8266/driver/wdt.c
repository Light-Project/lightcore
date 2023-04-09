/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/watchdog/esp8266.h>
#include <asm/io.h>

void __noreturn panic_reboot(void)
{
    writel(WDT_BASE + ESP8266_WDT_OPND, 0x10);
    writel(WDT_BASE + ESP8266_WDT_OP, 0x00);
    writel(WDT_BASE + ESP8266_WDT_CTL, ESP8266_WDT_CTL_EN);

    /* Wait wdt reboot */
    for (;;)
    halt();
}
