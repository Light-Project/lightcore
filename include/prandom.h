/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PRANDOM_H_
#define _PRANDOM_H_

#include <types.h>
#include <percpu.h>

struct prandom_state {
    uint32_t s1, s2;
    uint32_t s3, s4;
};

#define TAUSWORTHE(seed, a, b, c, d) \
    ((((seed) & (c)) << (d)) ^ ((((seed) << (a)) ^ (seed)) >> (b)))

DECLARE_PERCPU(struct prandom_state, prandom_state);

extern uint32_t prandom_state_value(struct prandom_state *pstate);
extern void prandom_state_bytes(struct prandom_state *pstate, void *buff, size_t len);
extern void prandom_state_seed(struct prandom_state *pstate, uint64_t seed);

extern uint32_t prandom_value(void);
extern void prandom_bytes(void *buff, size_t len);
extern void prandom_seed(uint64_t seed);

#endif  /* _PRANDOM_H_ */
