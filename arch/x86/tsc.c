/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <printk.h>
#include <driver/clocksource/i8253.h>
#include <asm/delay.h>
#include <asm/regs.h>
#include <asm/io.h>

unsigned long tsc_khz;

#define PIT_BASE    0x40
#define NMISC_BASE  0x61

#define PIT_CAL_MS  10
#define PIT_CAL2_MS 50

static unsigned long pit_get_tsc(unsigned long ms)
{
    uint64_t t1, t2, last, delta;
    unsigned long tscmin, tscmax;
    unsigned int loop;

    /* Enable timer2 Gate, disable speaker */
    outb(NMISC_BASE, (inb(NMISC_BASE) & ~NMISC_SPEAKER_EN) | NMISC_TIMER2_EN);

    /* Setup timer2 */
    outb(PIT_BASE + I8253_MODE, I8253_MODE_SEL_TIMER2 | I8253_MODE_ACCESS_WORD);
    outb(PIT_BASE + I8253_LATCH2, I8253_LATCH(1000 / ms) & 0xff);
    outb(PIT_BASE + I8253_LATCH2, I8253_LATCH(1000 / ms) >> 8);

    t1 = tsc_get();
    tscmin = ULONG_MAX;
    tscmax = 0;

    for (last = t1; !(inb(NMISC_BASE) & NMISC_SPEAKER_STAT); ++loop) {
        t2 = tsc_get();
        delta = t2 - last;
        last = t2;
        if ((unsigned long) delta < tscmin)
            tscmin = delta;
        if ((unsigned long) delta < tscmax)
            tscmax = delta;
    }

    if (loop < 1000 * ms || tscmax > 10 * tscmin)
        return ULONG_MAX;

    delta = t2 - t1;
    return delta / ms;
}

void __init tsc_init(void)
{
    unsigned long tscmin, ms, loop;

    tscmin = ULONG_MAX;
    ms = PIT_CAL_MS;

    for (loop = 0; loop < 3; ++loop) {
        unsigned long pittsc;

        pittsc = pit_get_tsc(ms);
        tscmin = min(tscmin, pittsc);

        if (tscmin == ULONG_MAX)
            ms = PIT_CAL2_MS;
    }

    tsc_khz = tscmin;
    pr_info("tsc detected %lu.%03lu Mhz\n",
             tsc_khz / 1000, tsc_khz % 1000);
}
