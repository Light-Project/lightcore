/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kclock"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kclock.h>
#include <spinlock.h>
#include <gunit.h>
#include <seqlock.h>
#include <ticktime.h>
#include <bitops.h>
#include <clockcloc.h>
#include <printk.h>
#include <export.h>

static SPIN_LOCK(kclock_lock);
static SEQ_LOCK(kclock_seq);

static uint64_t kclock_systick_read(void);
static struct kclock_data kclock = {
    .read = kclock_systick_read,
    .mult = NSEC_PER_SEC / SYSTICK_FREQ,
    .mask = UINT64_MAX,
};

static uint64_t kclock_systick_read(void)
{
    return (uint64_t)(ticktime - INITIAL_TTIME);
}

static inline uint64_t kclock_get_delta(struct kclock_data *kclock)
{
    uint64_t cycle, nsecs;

    cycle = kclock->read();
    cycle = (cycle - kclock->cycle_last) & kclock->mask;
    nsecs = clock_cycle_to_nsec(cycle, kclock->mult, kclock->shift);

    return nsecs;
}

static uint64_t kclock_forward(struct kclock_data *kclock)
{
    uint64_t cycle_now, nsecs;

    cycle_now = kclock->read();
    nsecs = kclock->base + clock_cycle_to_nsec((cycle_now - kclock->cycle_last) &
            kclock->mask, kclock->mult, kclock->shift);

    kclock->base = nsecs;
    return cycle_now;
}

void kclock_register(uint64_t (*read)(void), uint64_t freq, uint64_t mask)
{
    unsigned int mult, shift;
    irqflags_t irqflags;
    bool changed;

    if (READ_ONCE(kclock.freq) > freq)
        return;

    clock_cloc_mult_shift(&mult, &shift, freq, NSEC_PER_SEC, 3600);
    spin_lock_irqsave(&kclock_lock, &irqflags);
    seqlock_write_start(&kclock_seq);

    if (unlikely(!(changed = !(kclock.freq > freq))))
        goto finish;

    kclock_forward(&kclock);
    kclock.cycle_last = read();
    kclock.read = read;

    kclock.freq = freq;
    kclock.mask = mask;
    kclock.mult = mult;
    kclock.shift = shift;

finish:
    seqlock_write_end(&kclock_seq);
    spin_unlock_irqrestore(&kclock_lock, &irqflags);

    if (changed) {
        char gbuff[GFREQ_BUFF];
        uint64_t granu;

        granu = clock_cycle_to_nsec(1ULL, mult, shift);
        gfreq(gbuff, freq);

        pr_info("%s in %u bits, granularity %lluns\n", gbuff, fls(mask), granu);
        pr_debug("registered %p as kclock source\n", read);
    }
}
EXPORT_SYMBOL(kclock_register);

void kclock_update(void)
{
    irqflags_t irqflags;
    uint64_t cycle_now;

    spin_lock_irqsave(&kclock_lock, &irqflags);
    seqlock_write_start(&kclock_seq);

    cycle_now = kclock_forward(&kclock);
    kclock.cycle_last = cycle_now;

    seqlock_write_end(&kclock_seq);
    spin_unlock_irqrestore(&kclock_lock, &irqflags);
}
EXPORT_SYMBOL(kclock_update);

const struct kclock_data *kclock_read_start(unsigned int *seq)
{
    *seq = seqlock_read_start(&kclock_seq);
    return &kclock;
}
EXPORT_SYMBOL(kclock_read_start);

bool kclock_read_retry(unsigned int seq)
{
    return seqlock_read_retry(&kclock_seq, seq);
}
EXPORT_SYMBOL(kclock_read_retry);

uint64_t kclock_get(void)
{
    unsigned int seq;
    uint64_t nsec;

    do {
        seq = seqlock_read_start(&kclock_seq);
        nsec = kclock_get_delta(&kclock);
        nsec += kclock.base;
    } while (seqlock_read_retry(&kclock_seq, seq));

    return nsec;
}
EXPORT_SYMBOL(kclock_get);
