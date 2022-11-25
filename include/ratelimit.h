/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RATELIMIT_H_
#define _RATELIMIT_H_

#include <spinlock.h>
#include <ticktime.h>
#include <bitflags.h>

#define DEFAULT_RATELIMIT_INTERVAL  (5 * SYSTICK_FREQ)
#define DEFAULT_RATELIMIT_BURST     10

enum ratelimit_flags {
    __RATELIMIT_MSGQUIET    = 0,
};

#define RATELIMIT_MSGQUIET  BIT(__RATELIMIT_MSGQUIET)

/**
 * struct ratelimit - describe ratelimit status.
 * @begin: start time of current management granularity.
 * @interval: manage time granularity of the maximum number of transfers.
 * @burst: maximum number of transfers allowed in @interval period.
 * @passed: number of data passed in compliance with rate limit.
 * @missed: number of lost data exceeding the rate limit.
 */
struct ratelimit {
    spinlock_t lock;
    ttime_t begin;
    ttime_t interval;
    unsigned int burst;
    unsigned int passed;
    unsigned int missed;
    unsigned long flags;
};

#define RATELIMIT_STATIC(INTERVAL, BURST) \
    {.interval = INTERVAL, .burst = BURST}

#define RATELIMIT_INIT(INTERVAL, BURST) \
    (struct ratelimit) RATELIMIT_STATIC(INTERVAL, BURST)

#define DEFINE_RATELIMIT(NAME, INTERVAL, BURST) \
    struct ratelimit NAME = RATELIMIT_INIT(INTERVAL, BURST)

#define DEFAULT_RATELIMIT(NAME) \
    DEFINE_RATELIMIT(NAME, DEFAULT_RATELIMIT_INTERVAL, DEFAULT_RATELIMIT_BURST)

GENERIC_STRUCT_BITOPS(ratelimit, struct ratelimit, flags, false)
GENERIC_STRUCT_FLAG(ratelimit, struct ratelimit, flags, msgquiet, __RATELIMIT_MSGQUIET)

extern bool ratelimit(struct ratelimit *limit, const char *prompt);
extern void ratelimit_exit(struct ratelimit *limit, const char *prompt);
extern void ratelimit_init(struct ratelimit *limit, ttime_t interval, unsigned int burst);

static inline void ratelimit_default_init(struct ratelimit *limit)
{
    ratelimit_init(limit, DEFAULT_RATELIMIT_INTERVAL, DEFAULT_RATELIMIT_BURST);
}

#endif  /* _RATELIMIT_H_ */
