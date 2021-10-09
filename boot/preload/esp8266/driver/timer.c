/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <asm/io.h>
#include <driver/clocksource/timer-esp8266.h>

#define TIME_CLK_FREQ   (APB_CLK_FREQ >> 8)
#define TIMER_BASE 0x60000600
#define TICK_GRAIN 200

/* The grain size is 5ms */
uint32_t tick_time = 0;

void timer_handle(void)
{
    tick_time++;
}

void timer_delay(unsigned int ms)
{
    ms /= (1000 / TICK_GRAIN);
    ms += tick_time;
    while(ms != tick_time);
}

int time_read(void)
{
    return tick_time * (1000 / TICK_GRAIN);
}

void timer_init(void)
{
    uint32_t val;

    val = TIME_CLK_FREQ / TICK_GRAIN;
    writel((void *)TIMER_BASE + TIMER_ESP8266_LOAD, val);
    writel((void *)TIMER_BASE + TIMER_ESP8266_COUNT, val);

    val = TIMER_ESP8266_CTRL_EN | TIMER_ESP8266_CTRL_AUTOLOAD | TIMER_ESP8266_CTRL_DIV_256;
    writel((void *)TIMER_BASE + TIMER_ESP8266_CTRL, val);
}

void timer_stop(void)
{
    writel((void *)TIMER_BASE + TIMER_ESP8266_CTRL, 0);
}
