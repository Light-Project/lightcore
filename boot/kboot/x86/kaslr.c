/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <driver/clocksource/i8253.h>

#include <asm/io.h>

/**
 * rdtsc_random - get random number from rdtsc
 */
static __always_inline
uint64_t rdtsc_random(void)
{
    uint32_t reth, retl;
    asm volatile("rdtsc" : "=Rd"(reth), "=Ra"(retl));
    return ((uint64_t)reth << 32) | retl;
}

/**
 * i8253_random - get random number from I8253
 */
static __always_inline
uint16_t i8253_random(void)
{
    uint16_t val;
    uint8_t status;

    do {
        outb(I8253_MODE, I8253_MODE_SEL_READBACK | 
            I8253_READ_COUNTER0);
        status = inb(I8253_COUNTER0);
        val = inb(I8253_COUNTER0);
        val |= inb(I8253_COUNTER0) << 8;
    } while(status & I8253_STATUS_NULL);

    return val;
}

uint64_t kasir_random(void)
{
    uint64_t random = i8253_random();

    random ^= rdtsc_random();

    return random;
}
