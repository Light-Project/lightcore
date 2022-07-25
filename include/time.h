/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIME_H_
#define _TIME_H_

#include <types.h>
#include <limits.h>

/* Parameters used to convert the timespec values */
#define MSEC_PER_SEC    1000UL
#define USEC_PER_MSEC   1000UL
#define NSEC_PER_USEC   1000UL
#define NSEC_PER_MSEC   1000000UL
#define USEC_PER_SEC    1000000UL
#define NSEC_PER_SEC    1000000000UL
#define PSEC_PER_SEC    1000000000000ULL
#define FSEC_PER_SEC    1000000000000000ULL

#define TTIME_MAX       INT64_MAX
#define TTIME_MIN       INT64_MIN

#define KTIME_MAX       INT64_MAX
#define KTIME_MIN       INT64_MIN
#define KTIME_SEC_MAX   (KTIME_MAX / NSEC_PER_SEC)
#define KTIME_SEC_MIN   (KTIME_MIN / NSEC_PER_SEC)

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

extern time_t mktime(unsigned int year, unsigned int mon, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec);
extern struct timespec ns_to_timespec(const int64_t nsec);
extern int64_t timespec_to_ns(struct timespec *ts);

#endif  /* _TIME_H_ */
