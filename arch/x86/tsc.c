/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <gunit.h>
#include <delay.h>
#include <printk.h>
#include <driver/clocksource/i8253.h>
#include <driver/platform/ibmpc.h>
#include <asm/io.h>
#include <asm/regs.h>

#define PIT_BASE        0x40
#define PIT_CAL1_MS     10
#define PIT_CAL1_LOOPS  1000
#define PIT_CAL2_MS     100
#define PIT_CAL2_LOOPS  10000

static unsigned long pit_get_tsc(unsigned long ms, unsigned int minloop)
{
    uint64_t t1, t2, last, delta;
    unsigned long tscmin, tscmax;
    unsigned int loop;
    uint8_t val;

    /* enable timer2 gate, disable speaker */
    val = inb(IBMPC_NMISC_BASE);
    val = (val & ~IBMPC_NMISC_SPEAKER_EN) | IBMPC_NMISC_TIMER2_EN;
    outb(IBMPC_NMISC_BASE, val);

    /*
     * Counter Select: Counter 2 select
     * Read/Write Select: Read/Write LSB then MSB
     * Counter Mode: Out signal on end of count
     * Binary/BCD Select: Binary countdown is used
     */
    outb(PIT_BASE + I8253_MODE, I8253_MODE_SEL_TIMER2 | I8253_MODE_ACCESS_WORD);
    outb(PIT_BASE + I8253_LATCH2, I8253_LATCH(1000 / ms) & 0xff);
    outb(PIT_BASE + I8253_LATCH2, I8253_LATCH(1000 / ms) >> 8);

    last = t2 = t1 = tsc_get();
    tscmin = ULONG_MAX;
    tscmax = 0;

    for (loop = 0; !(inb(IBMPC_NMISC_BASE) & IBMPC_NMISC_SPEAKER_STAT); ++loop) {
        t2 = tsc_get();
        delta = t2 - last;
        last = t2;
        if ((unsigned long) delta < tscmin)
            tscmin = delta;
        if ((unsigned long) delta < tscmax)
            tscmax = delta;
    }

    /*
     * Sanity checks:
     *
     * If we were not able to read the PIT more than loopmin
     * times, then we have been hit by a massive SMI
     *
     * If the maximum is 10 times larger than the minimum,
     * then we got hit by an SMI as well.
     */
    if (loop < minloop || tscmax > 10 * tscmin)
        return ULONG_MAX;

    /* Calculate the PIT value */
    delta = t2 - t1;
    return delta / ms;
}

void __init arch_tsc_setup(void)
{
    char gbuff[GFREQ_BUFF];
    unsigned long pittsc_min;
    unsigned long ms, minloop;
    unsigned int count;

    pittsc_min = ULONG_MAX;
    ms = PIT_CAL1_MS;
    minloop = PIT_CAL1_LOOPS;

    for (count = 0; count < 3; ++count) {
        unsigned long pittsc;

        pittsc = pit_get_tsc(ms, minloop);
        pittsc_min = min(pittsc_min, pittsc);

        if (count == 1 && pittsc_min == ULONG_MAX) {
            ms = PIT_CAL2_MS;
            minloop = PIT_CAL2_LOOPS;
        }
    }

    if (pittsc_min == ULONG_MAX)
        pr_warn("unable to calibrate against pit\n");

    loops_per_tick = pittsc_min;
    gfreq(gbuff, pittsc_min * 1000);
    pr_info("tsc detected %s\n", gbuff);
}
