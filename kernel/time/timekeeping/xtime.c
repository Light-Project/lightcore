/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

static inline void
timekeeping_xbase_normalized(struct timekeeper_base *tkb)
{
    uint64_t nsecps = (uint64_t)NSEC_PER_SEC << tkb->shift;

    while (tkb->xtime_nsec >= nsecps) {
        tkb->xtime_nsec -= nsecps;
        tkb->xtime_sec++;
    }
}

static inline void
timekeeping_xtime_normalized(struct timekeeper *tk)
{
    timekeeping_xbase_normalized(&tk->base_mono);
    timekeeping_xbase_normalized(&tk->base_raw);
}

static inline struct timespec
timekeeping_xtime_get(struct timekeeper *tk)
{
    struct timespec ts;
    ts.tv_sec = tk->base_mono.xtime_sec;
    ts.tv_nsec = tk->base_mono.xtime_nsec >> tk->base_mono.shift;
    return ts;
}

static inline void
timekeeping_xtime_set(struct timekeeper *tk, struct timespec *ts)
{
    tk->base_mono.xtime_sec = ts->tv_sec;
    tk->base_mono.xtime_nsec = ts->tv_nsec << tk->base_mono.shift;
}

static inline void
timekeeping_xtime_add(struct timekeeper *tk, struct timespec *ts)
{
    tk->base_mono.xtime_sec += ts->tv_sec;
    tk->base_mono.xtime_nsec += ts->tv_nsec << tk->base_mono.shift;
    timekeeping_xtime_normalized(tk);
}

static void timekeeping_xtime_forward(struct timekeeper *tk)
{
    struct timekeeper_base *base = &tk->base_mono;
    uint64_t delta, cycle_now;

    cycle_now = timekeeping_base_get_cycle(base);
    delta = timekeeping_base_cycle_to_delta(cycle_now, base->cycle_last, base->mask);
    tk->base_mono.cycle_last = cycle_now;
    tk->base_raw.cycle_last = cycle_now;

    tk->base_mono.xtime_nsec = delta * tk->base_mono.mult;
    tk->base_raw.xtime_nsec = delta * tk->base_raw.mult;
    timekeeping_xtime_normalized(tk);
}
