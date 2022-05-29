/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <prandom.h>
#include <cpu.h>
#include <bytefill.h>
#include <export.h>

DEFINE_PERCPU(struct prandom_state, prandom_state);
EXPORT_SYMBOL(prandom_state);

static __always_inline uint32_t prandom_seed_minimum(uint32_t x, uint32_t m)
{
    return (x < m) ? x + m : x;
}

void prandom_state_seed(struct prandom_state *pstate, uint64_t seed)
{
    seed = ((seed >> 32) ^ (seed << 10) ^ seed) & 0xffffffffUL;
    pstate->s1 = prandom_seed_minimum(seed,   2U);
    pstate->s2 = prandom_seed_minimum(seed,   8U);
    pstate->s3 = prandom_seed_minimum(seed,  16U);
    pstate->s4 = prandom_seed_minimum(seed, 128U);
}
EXPORT_SYMBOL(prandom_state_seed);

uint32_t prandom_state_value(struct prandom_state *pstate)
{
    pstate->s1 = TAUSWORTHE(pstate->s1,  6U, 13U, 4294967294U, 18U);
    pstate->s2 = TAUSWORTHE(pstate->s2,  2U, 27U, 4294967288U,  2U);
    pstate->s3 = TAUSWORTHE(pstate->s3, 13U, 21U, 4294967280U,  7U);
    pstate->s4 = TAUSWORTHE(pstate->s4,  3U, 12U, 4294967168U, 13U);
    return pstate->s1 ^ pstate->s2 ^ pstate->s3 ^ pstate->s4;
}
EXPORT_SYMBOL(prandom_state_value);

BYTEFILL_DECLARE_PREFIX(, prandom_state_bytes, uint32_t, prandom_state_value, (pstate), struct prandom_state *pstate)
EXPORT_SYMBOL(prandom_state_bytes);

uint32_t prandom_value(void)
{
    struct prandom_state *pstate = thiscpu_ptr(&prandom_state);
    return prandom_state_value(pstate);
}
EXPORT_SYMBOL(prandom_value);

void prandom_bytes(void *buff, size_t len)
{
    struct prandom_state *pstate = thiscpu_ptr(&prandom_state);
    return prandom_state_bytes(pstate, buff, len);
}
EXPORT_SYMBOL(prandom_bytes);

void prandom_seed(uint64_t seed)
{
    struct prandom_state *pstate;
    unsigned int cpu;

    cpu_for_each_possible(cpu) {
        pstate = percpu_ptr(cpu, &prandom_state);
        prandom_state_seed(pstate, seed);
    }
}
EXPORT_SYMBOL(prandom_seed);
