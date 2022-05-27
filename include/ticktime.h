/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TICKTIME_H_
#define _TICKTIME_H_

#include <time.h>
#include <math.h>

extern volatile ttime_t ticktime;
#define SYSTICK_FREQ CONFIG_SYSTICK_FREQ
#define SYSTICK_NSEC DIV_ROUND_CLOSEST(NSEC_PER_SEC, SYSTICK_FREQ)

#define ttime_add(ttime, time)  ((ttime) + (time))
#define ttime_sub(ttime, time)  ((ttime) - (time))

/**
 * ttime_to_ns/us/ms - convert ttime to time
 * @time: the ttime to convert
 */
static inline int64_t ttime_to_ns(ttime_t time)
{
    return time * SYSTICK_NSEC;
}

static inline int64_t ttime_to_us(ttime_t time)
{
    return ttime_to_ns(time) / NSEC_PER_USEC;
}

static inline int64_t ttime_to_ms(ttime_t time)
{
    return ttime_to_ns(time) / NSEC_PER_MSEC;
}

/**
 * ns/us/ms_to_ttime - convert time to ttime
 * @time: the ns/us/ms to convert
 */
static inline ttime_t ns_to_ttime(int64_t time)
{
    return DIV_ROUND_UP(time, SYSTICK_NSEC);
}

static inline ttime_t us_to_ttime(int64_t time)
{
    return ns_to_ttime(time * NSEC_PER_USEC);
}

static inline ttime_t ms_to_ttime(int64_t time)
{
    return ns_to_ttime(time * NSEC_PER_MSEC);
}

/**
 * ttime_compare - compares two ttime_t variables
 * @timea: variables a
 * @timeb: variables b
 */
static inline int ttime_compare(const ttime_t timea, const ttime_t timeb)
{
    if (timea < timeb)
        return -1;
    if (timea > timeb)
        return 1;
    return 0;
}

/**
 * ttime_after - compare if a ttime value is bigger than another one
 * @timea: the bigger one
 * @timeb: the smaller one
 */
static inline bool ttime_after(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) > 0;
}

/**
 * ttime_before - compare if a ttime value is smaller than another one
 * @timea: the smaller one
 * @timeb: the bigger one
 */
static inline bool ttime_before(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) < 0;
}

/**
 * ttime_after - compare if a ttime value is bigger or equal than another one
 * @timea: the bigger one
 * @timeb: the smaller one
 */
static inline bool ttime_after_equal(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) >= 0;
}

/**
 * ttime_before_equal - compare if a ttime value is smaller than or equal another one
 * @timea: the smaller one
 * @timeb: the bigger one
 */
static inline bool ttime_before_equal(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) <= 0;
}

#endif  /* _TICKTIME_H_ */
