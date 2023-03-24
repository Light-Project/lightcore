/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMERKEEPING_H_
#define _TIMERKEEPING_H_

#include <ktime.h>
#include <ticktime.h>

struct clocksource_device;
struct clockevent_device;

enum timekeeper_offsets {
    TIMEKEEPER_BOOT,
    TIMEKEEPER_REAL,
    TIMEKEEPER_TAI,
    TIMEKEEPER_NR_MAX,
};

/**
 * timekeeper_base - timekeeping readout base struct.
 * @cdev: current clock source pointer.
 * @base: base offset of clock source to ktime.
 * @mask: bitmask for complement subtraction of non 64bit clocks.
 * @cycle_last: last updated clock source cycle.
 * @mult: multiplier for scaled math conversion.
 * @shift: shift value for scaled math conversion.
 * @xtime_nsec: shifted nano seconds offset.
 */
struct timekeeper_base {
    struct clocksource_device *cdev;

    /* cycle to delta */
    ktime_t base;
    uint64_t mask;
    uint64_t cycle_last;

    /* delta to nsec */
    unsigned int mult;
    unsigned int shift;

    uint64_t xtime_sec;
    uint64_t xtime_nsec;
    uint64_t xtime_interval;
};

/**
 * timekeeper - base struct for timekeeping readout.
 * @base_mono: system base monotonic timer (self calibration).
 * @base_raw: system base monotonic timer (hardware raw).
 * @offset_boot: offset of monotonic to boottime.
 * @offset_real: offset of monotonic to realtime.
 * @offset_tai: offset of monotonic to taitime.
 * @tai_offset: offset of realtime to taitime.
 * @systick_cycle: number of cycles in a systick.
 */
struct timekeeper {
    struct timekeeper_base base_mono;
    struct timekeeper_base base_raw;

    /* use to ktime convert */
    ktime_t offset_boot;
    ktime_t offset_real;
    ktime_t offset_tai;
    int64_t tai_offset;

    /* use to timespec convert */
    struct timespec timespec_boot;
    struct timespec timespec_real;

    uint64_t systick_cycle;
    unsigned long ktime_sec;
};

/* kernel time get */
extern ktime_t timekeeping_get_time(void);
extern ktime_t timekeeping_get_raw(void);
extern ktime_t timekeeping_get_offset(enum timekeeper_offsets index);

/**
 * timekeeping_get_boot/real/taitime - get time in ktime_t format
 * @return: ktime_t format time
 */
static inline ktime_t timekeeping_get_boottime(void)
{
    return timekeeping_get_offset(TIMEKEEPER_BOOT);
}

static inline ktime_t timekeeping_get_realtime(void)
{
    return timekeeping_get_offset(TIMEKEEPER_REAL);
}

static inline ktime_t timekeeping_get_clocktai(void)
{
    return timekeeping_get_offset(TIMEKEEPER_TAI);
}

/**
 * timekeeping_get_boot/real/taitime_ts - get time in timespec format
 * @return: timespec format time
 */
static inline struct timespec timekeeping_get_time_ts(void)
{
    return ktime_to_timespec(timekeeping_get_time());
}

static inline struct timespec timekeeping_get_boottime_ts(void)
{
    return ktime_to_timespec(timekeeping_get_boottime());
}

static inline struct timespec timekeeping_get_realtime_ts(void)
{
    return ktime_to_timespec(timekeeping_get_realtime());
}

/**
 * timekeeping_get_boot/real/taitime_ts - get time in nanosecond format
 * @return: nanosecond format time
 */
static inline uint64_t timekeeping_get_time_ns(void)
{
    return ktime_to_ns(timekeeping_get_time());
}

static inline uint64_t timekeeping_get_boottime_ns(void)
{
    return ktime_to_ns(timekeeping_get_boottime());
}

static inline uint64_t timekeeping_get_realtime_ns(void)
{
    return ktime_to_ns(timekeeping_get_realtime());
}

/* timekeeping core */
extern void timekeeping_tick(void);
extern state timekeeping_set_realtime(struct timespec *ts);
extern state timekeeping_change(struct clocksource_device *cdev);

/* systick core */
extern state systick_change(struct clockevent_device *cdev);

#endif /* _TIMERKEEPING_H_ */
