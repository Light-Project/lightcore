/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <timecounter.h>
#include <bits.h>
#include <export.h>

static inline uint64_t timecounter_get_delta(struct time_counter *cnt)
{
    struct time_source *src = cnt->src;
    uint64_t cycle, nsecs;

    cycle = src->read(cnt);
    cycle = (cycle - cnt->cycle_last) & src->mask;
    nsecs = clock_cycle_to_nsec(cycle, cnt->mask, src->mult, src->shift, &cnt->frac);
	cnt->cycle_last = cycle;

    return nsecs;
}

uint64_t timecounter_update(struct time_counter *cnt)
{
    uint64_t nsecs;

    nsecs = timecounter_get_delta(cnt);
    nsecs += cnt->nsec;
    cnt->nsec = nsecs;

    return nsecs;
}
EXPORT_SYMBOL(timecounter_update);

void timercounter_init(struct time_counter *cnt, struct time_source *src, uint64_t start)
{
    cnt->src = src;
    cnt->nsec = start;
    cnt->mask = BIT_LOW_MASK(src->shift);
    cnt->cycle_last = src->read(cnt);
}
EXPORT_SYMBOL(timercounter_init);
