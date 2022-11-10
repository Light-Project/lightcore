/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTIME_H_
#define _KTIME_H_

#include <types.h>
#include <math.h>
#include <time.h>

#define ktime_to_timespec(ns)   ns_to_timespec(ns)
#define timespec_to_ktime(ts)   timespec_to_ns(ts)
#define ktime_add(time, nsval)  ((time) + (nsval))
#define ktime_sub(time, nsval)  ((time) - (nsval))

/**
 * ktime_to_ns/us/ms - convert ktime to time.
 * @time: the ktime to convert.
 */
static inline int64_t ktime_to_ns(ktime_t time)
{
    return time;
}

static inline int64_t ktime_to_us(ktime_t time)
{
    return ktime_to_ns(time) / NSEC_PER_USEC;
}

static inline int64_t ktime_to_ms(ktime_t time)
{
    return ktime_to_ns(time) / NSEC_PER_MSEC;
}

/**
 * ns/us/ms_to_ktime - convert time to ktime.
 * @time: the ns/us/ms to convert.
 */
static inline ktime_t ns_to_ktime(int64_t time)
{
    return time;
}

static inline ktime_t us_to_ktime(int64_t time)
{
    return ns_to_ktime(time * NSEC_PER_USEC);
}

static inline ktime_t ms_to_ktime(int64_t time)
{
    return ns_to_ktime(time * NSEC_PER_MSEC);
}

/**
 * ktime_set - set a ktime from a seconds/nanoseconds value.
 * @secs: seconds to set.
 * @nsec: nanoseconds to set.
 */
static inline ktime_t ktime_set(time_t secs, long nsec)
{
    if (unlikely(secs >= KTIME_SEC_MAX))
        return KTIME_MAX;

    return ns_to_ktime(secs * NSEC_PER_SEC + nsec);
}

/**
 * ktime_add_ns/us/ms - add time to ktime.
 * @time: the ktime to add.
 * @nsec: the ns/us/ms to add.
 */
static inline ktime_t ktime_add_ns(const ktime_t time, const uint64_t nsec)
{
    return ktime_add(time, nsec);
}

static inline ktime_t ktime_add_us(const ktime_t time, const uint64_t usec)
{
    return ktime_add_ns(time, usec * NSEC_PER_USEC);
}

static inline ktime_t ktime_add_ms(const ktime_t time, const uint64_t msec)
{
    return ktime_add_ns(time, msec * NSEC_PER_MSEC);
}

/**
 * ktime_sub_ns/us/ms - sub time form ktime
 * @time: the ktime to sub
 * @nsec: the ns/us/ms to sub
 */
static inline ktime_t ktime_sub_ns(const ktime_t time, const uint64_t nsec)
{
    return ktime_sub(time, nsec);
}

static inline ktime_t ktime_sub_us(const ktime_t time, const uint64_t usec)
{
    return ktime_sub_ns(time, usec * NSEC_PER_USEC);
}

static inline ktime_t ktime_sub_ms(const ktime_t time, const uint64_t msec)
{
    return ktime_sub_ns(time, msec * NSEC_PER_MSEC);
}

/**
 * ktime_compare - compares two ktime_t variables.
 * @timea: variables a.
 * @timeb: variables b.
 */
static inline int ktime_compare(const ktime_t timea, const ktime_t timeb)
{
    if (timea < timeb)
        return -1;
    if (timea > timeb)
        return 1;
    return 0;
}

/**
 * ktime_after - compare if a ktime value is bigger than another one.
 * @timea: the bigger one.
 * @timeb: the smaller one.
 */
static inline bool ktime_after(const ktime_t timea, const ktime_t timeb)
{
    return ktime_compare(timea, timeb) > 0;
}

/**
 * ktime_before - compare if a ktime value is smaller than another one.
 * @timea: the smaller one.
 * @timeb: the bigger one.
 */
static inline bool ktime_before(const ktime_t timea, const ktime_t timeb)
{
    return ktime_compare(timea, timeb) < 0;
}

/**
 * ktime_make - make ktime value from seconds/nanoseconds.
 * @secs: seconds to make.
 * @nsecs: nanoseconds to make.
 */
static inline ktime_t ktime_make(const uint64_t secs, const unsigned long nsecs)
{
    if (unlikely(secs >= KTIME_SEC_MAX))
        return KTIME_MAX;

    return secs * NSEC_PER_SEC + nsecs;
}

#endif  /* _KTIME_H_ */
