/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "timekeeping"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <timekeeping.h>
#include <driver/clocksource.h>
#include <export.h>

volatile ttime_t ticktime;

static bool __read_mostly timekeeping_ready;
static struct timekeeper timekeeper;
static SEQ_LOCK(timekeeper_lock);

static ktime_t *timekeeper_offsets[TIMEKEEPER_NR_MAX] = {
    [TIMEKEEPER_BOOT] = &timekeeper.offset_boot,
    [TIMEKEEPER_REAL] = &timekeeper.offset_real,
    [TIMEKEEPER_TAI]  = &timekeeper.offset_tai,
};

/**
 * timekeeping_base_cycle_to_delta - convert timekeeper cycle number to cycle delta
 * @now: current number of clock cycles
 * @last: last number of clock cycles
 * @mask: maximum value of clock
 */
static inline uint64_t timekeeping_base_cycle_to_delta(uint64_t now, uint64_t last, uint64_t mask)
{
    uint64_t ret = (now - last) & mask;
    return ret & ~(mask >> 1) ? 0 : ret;
}

/**
 * timekeeping_base_delta_to_ns - convert timekeeper cycle delta to nsec delta
 * @tkb: point to timekeeper base
 * @delta: the cycle delta to convert
 */
static inline uint64_t timekeeping_base_delta_to_ns(struct timekeeper_base *tkb, uint64_t delta)
{
    uint64_t nsec;
    nsec = delta * tkb->mult + tkb->rnsec;
    nsec >>= tkb->shift;
    return nsec;
}

/**
 * timekeeping_base_get_cycle - get timekeeper cycle number
 * @tkb: point to timekeeper base
 */
static inline uint64_t timekeeping_base_get_cycle(const struct timekeeper_base *tkb)
{
    struct clocksource_device *cdev = tkb->cdev;
    return cdev->ops->read(cdev);
}

/**
 * timekeeping_base_get_delta - get timekeeper cycle delta
 * @tkb: point to timekeeper base
 */
static inline uint64_t timekeeping_base_get_delta(const struct timekeeper_base *tkb)
{
    uint64_t cycle, delta;
    cycle = timekeeping_base_get_cycle(tkb);
    delta = timekeeping_base_cycle_to_delta(cycle, tkb->cycle_last, tkb->mask);
    return delta;
}

/**
 * timekeeping_base_get_ns - get timekeeper nsec delta
 * @tkb: point to timekeeper base
 */
static inline uint64_t timekeeping_base_get_ns(struct timekeeper_base *tkb)
{
    uint64_t delta, nsec;
    delta = timekeeping_base_get_delta(tkb);
    nsec = timekeeping_base_delta_to_ns(tkb, delta);
    return nsec;
}

/**
 * timekeeping_base_setup - setup bases to new clocksource clock
 * @tk: the timekeeper to setup
 * @cdev: new clocksource pointer
 */
static void timekeeping_base_setup(struct timekeeper *tk, struct clocksource_device *cdev)
{
    struct clocksource_device *ocdev;
    uint64_t interval, cycle_now;

    ocdev = tk->base_mono.cdev;
    tk->base_mono.cdev = cdev;
    tk->base_raw.cdev = cdev;

    cycle_now = timekeeping_base_get_cycle(&tk->base_mono);
    tk->base_mono.cycle_last = cycle_now;
    tk->base_raw.cycle_last = cycle_now;

    interval = SYSTICK_NSEC;
    interval <<= cdev->shift;
    interval = DIV_ROUND_CLOSEST(interval, cdev->mult);

    tk->systick_cycle = interval;
    tk->second_cycle = interval * cdev->mult;

    if (ocdev) {
        int change = cdev->shift - ocdev->shift;
        if (change < 0) {
            tk->base_mono.rnsec >>= -change;
            tk->base_raw.rnsec >>= -change;
        } else if (change > 0) {
            tk->base_mono.rnsec <<= change;
            tk->base_raw.rnsec <<= change;
        }
    }

    tk->base_mono.mask = cdev->mask;
    tk->base_mono.mult = cdev->mult;
    tk->base_mono.shift = cdev->shift;
    tk->base_raw.mask = cdev->mask;
    tk->base_raw.mult = cdev->mult;
    tk->base_raw.shift = cdev->shift;
}

static void accumulate_nsecs_to_secs(struct timekeeper *tk)
{
    uint64_t nsecps = (uint64_t)NSEC_PER_SEC << tk->base_mono.cdev->shift;

    while (tk->base_mono.rnsec >= nsecps) {
        tk->base_mono.rnsec -= nsecps;
        tk->rsecs++;
    }
}

static uint64_t logarithmic_accumulation(struct timekeeper *tk, uint64_t offset, int shift)
{
    uint64_t interval = tk->systick_cycle << shift;

    if (offset < interval)
        return offset;

    /* accumulate one shifted interval */
    offset -= interval;
    tk->base_mono.cycle_last += interval;
    tk->base_raw.cycle_last += interval;

    tk->base_mono.rnsec += tk->second_cycle << shift;
    accumulate_nsecs_to_secs(tk);

    return offset;
}

static void timekeeping_base_update(struct timekeeper *tk, uint64_t offset)
{
    int shift;

    shift = ilog2(offset) - ilog2(tk->systick_cycle);
    shift = max(0, shift);

    while (offset >= tk->systick_cycle) {
        offset = logarithmic_accumulation(tk, offset, shift);
        if (offset < tk->systick_cycle << shift)
            shift--;
    }
}

static void timekeeping_adjustment(struct timekeeper *tk, bool systick)
{
    uint64_t offset;

    /* check whether the clock needs to be updated */
    offset = timekeeping_base_get_cycle(&tk->base_mono);
    if (offset < tk->systick_cycle && systick)
        return;

    timekeeping_base_update(tk, offset);
}

/**
 * timekeeping_tick - timekeeping systick handle
 *
 */
void timekeeping_tick(void)
{
    ticktime++;
    return;
    timekeeping_adjustment(&timekeeper, true);
}

/**
 * timekeeping_change
 *
 */
state timekeeping_change(struct clocksource_device *cdev)
{
    timekeeping_base_setup(&timekeeper, cdev);
    timekeeping_ready = true;
    return -ENOERR;
}

/**
 * timekeeping_set_realtime -
 *
 */
state timekeeping_set_realtime(struct timespec *ts)
{
    return -ENOERR;
}

/**
 * timekeeping_convert_offset -
 *
 */
ktime_t timekeeping_convert_offset(ktime_t tmono, enum timekeeper_offsets index)
{
    ktime_t offset = *timekeeper_offsets[index];
    ktime_t tconv;
    unsigned int seq;

    do {
        seq = seqlock_read_start(&timekeeper_lock);
        tconv = ktime_add(tmono, offset);
    } while (seqlock_read_retry(&timekeeper_lock, seq));

    return tconv;
}
EXPORT_SYMBOL(timekeeping_convert_offset);

/**
 * timekeeping_get_offset -
 *
 */
ktime_t timekeeping_get_offset(enum timekeeper_offsets index)
{
    ktime_t offset = *timekeeper_offsets[index];
    ktime_t base;
    uint64_t delta;
    unsigned int seq;

    if (!timekeeping_ready)
        return 0;

    do {
        seq = seqlock_read_start(&timekeeper_lock);
        base = ktime_add(timekeeper.base_mono.base, offset);
        delta = timekeeping_base_get_ns(&timekeeper.base_mono);
    } while (seqlock_read_retry(&timekeeper_lock, seq));

    return ktime_add_ns(base, delta);
}
EXPORT_SYMBOL(timekeeping_get_offset);

/**
 * timekeeping_get_time -
 *
 */
ktime_t timekeeping_get_time(void)
{
    ktime_t base;
    uint64_t delta;
    unsigned int seq;

    if (!timekeeping_ready)
        return 0;

    do {
        seq = seqlock_read_start(&timekeeper_lock);
        base = timekeeper.base_mono.base;
        delta = timekeeping_base_get_ns(&timekeeper.base_mono);
    } while (seqlock_read_retry(&timekeeper_lock, seq));

    return ktime_add_ns(base, delta);
}
EXPORT_SYMBOL(timekeeping_get_time);
