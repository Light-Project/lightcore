/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  bootloader/x86/driver/timer.c
 *  x86 pre timer driver
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#include <driver/platform.h>
#include <driver/clocksource/i8253.h>

static void timer_handle()
{
    
}

static void timer_init(void)
{
    /* Set timer 0 mode first */
    io_out8(PIT_MODE,                   // Mode/Command register
            PIT_MODE_SEL_TIMER0  |      // Channel 0
            PIT_MODE_ACCESS_WORD |      // low byte / high byte
            PIT_MODE_MOD_RATE    |      // rate generator
            PIT_MODE_CNT_BINARY);       // binary count

    /* Setting frequency division coefficient */
    io_out8(PIT_COUNTER0, 0x00);
    io_out8(PIT_COUNTER0, 0x00);

    /* link timer 0 interrupt */
    irq_alloc(0x20, timer_handle);
}


