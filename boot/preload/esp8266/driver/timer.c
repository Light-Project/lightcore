/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <linkage.h>
#include <driver/clocksource/esp8266.h>
#include <asm/proc.h>
#include <asm/io.h>

/* The grain size is 5ms */
#define TICK_GRAIN 200
uint32_t tick_time = 0;

asmlinkage void timer_handle(void)
{
    tick_time++;
}

void mdelay(uint32_t ms)
{
    ms /= (1000 / TICK_GRAIN);
    ms += tick_time;
    while (tick_time < ms)
        cpu_relax();
}

uint32_t time_read(void)
{
    return tick_time * (1000 / TICK_GRAIN);
}

void timer_init(void)
{
    uint32_t val;

    val = TIME_FREQ / TICK_GRAIN;
    writel(TIMER_BASE + ESP8266_TIMER_LOAD, val);
    writel(TIMER_BASE + ESP8266_TIMER_COUNT, val);

    val = ESP8266_TIMER_CTRL_EN | ESP8266_TIMER_CTRL_AUTOLOAD | ESP8266_TIMER_CTRL_DIV_256;
    writel(TIMER_BASE + ESP8266_TIMER_CTRL, val);
}

void timer_stop(void)
{
    writel(TIMER_BASE + ESP8266_TIMER_CTRL, 0);
}
