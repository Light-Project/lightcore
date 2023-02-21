/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMECOUNTER_H_
#define _TIMECOUNTER_H_

#include <types.h>
#include <clockcloc.h>

struct time_counter {
    struct time_source *src;
    uint64_t cycle_last;
    uint64_t mask;
    uint64_t frac;
    uint64_t nsec;
};

struct time_source {
    uint64_t (*read)(struct time_counter *cnt);
    uint64_t mask;
    unsigned int mult;
    unsigned int shift;
};

extern uint64_t timecounter_update(struct time_counter *cnt);
extern void timercounter_init(struct time_counter *cnt, struct time_source *src, uint64_t start);

#endif /* _TIMECOUNTER_H_ */
