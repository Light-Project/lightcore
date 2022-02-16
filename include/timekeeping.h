/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMERKEEPING_H_
#define _TIMERKEEPING_H_

#include <ktime.h>
#include <seqlock.h>

struct clocksource_device;
struct clockevent_device;

enum timekeeper_offsets {
    TIMEKEEPER_BOOT,
    TIMEKEEPER_REAL,
    TIMEKEEPER_TAI,
    TIMEKEEPER_NR_MAX,
};

/**
 * timekeeper_base - timekeeping readout base struct
 * @cdev: current clockdevice pointer
 * @mask:
 * @mult:
 * @shift:
 * @cycle_last: last updated clockdevice cycle
 * @base: offset of clockdevice to ktime
 */
struct timekeeper_base {
    struct clocksource_device *cdev;
    uint64_t mask;
    unsigned int mult;
    unsigned int shift;

    uint64_t cycle_last;
    uint64_t rnsec;
    ktime_t base;
};

/**
 * timekeeper - base struct for timekeeping readout
 * @offset_boot: offset of monotonic to boottime
 * @offset_real: offset of monotonic to realtime
 * @offset_tai: offset of monotonic to tai
 * @systick_cycle: number of cycles in a systick
 * @realtime_cycle: number of cycles in a second
 */
struct timekeeper {
    struct timekeeper_base base_mono;
    struct timekeeper_base base_raw;
    ktime_t offset_boot;
    ktime_t offset_real;
    ktime_t offset_tai;
    uint64_t systick_cycle;
    uint64_t second_cycle;
    uint64_t rsecs;
};

#define SYSTICK_NSEC        \
    DIV_ROUND_CLOSEST(      \
        NSEC_PER_SEC,       \
        CONFIG_SYSTICK_FREQ \
)

/* kernel time get */
extern ktime_t timekeeping_get_time(void);
extern ktime_t timekeeping_get_offset(enum timekeeper_offsets index);

/**
 * timekeeping_get_boottime - get boottime in ktime_t format
 */
static inline ktime_t timekeeping_get_boottime(void)
{
    return timekeeping_get_offset(TIMEKEEPER_BOOT);
}

/**
 * timekeeping_get_realtime - get realtime in ktime_t format
 */
static inline ktime_t timekeeping_get_realtime(void)
{
    return timekeeping_get_offset(TIMEKEEPER_REAL);
}

/**
 * timekeeping_get_clocktai - get tai clock in ktime_t format
 */
static inline ktime_t timekeeping_get_clocktai(void)
{
    return timekeeping_get_offset(TIMEKEEPER_TAI);
}

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

/* timekeeping core */
extern volatile uint64_t ticktime;
extern void timekeeping_tick(void);
extern state timekeeping_set_realtime(struct timespec *ts);
extern state timekeeping_change(struct clocksource_device *cdev);

/* systick core */
extern state systick_change(struct clockevent_device *cdev);

#endif  /* _TIMERKEEPING_H_ */
