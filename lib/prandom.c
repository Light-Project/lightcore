/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <prandom.h>
#include <preempt.h>
#include <cpu.h>
#include <bytefill.h>
#include <export.h>

DEFINE_PERCPU(struct prandom_state, prandom_state) = {
    .s1 = 0x408c3c09UL,
    .s2 = 0x408c3c09UL,
    .s3 = 0x408c3c09UL,
    .s4 = 0x408c3c09UL,
};
EXPORT_SYMBOL(prandom_state);

static __always_inline uint32_t
prandom_seed_minimum(uint32_t x, uint32_t m)
{
    return (x < m) ? x + m : x;
}

static void prandom_state_setup(struct prandom_state *pstate, uint64_t seed)
{
    seed = lower_32_bits((seed >> 32) ^ (seed << 10) ^ seed);
    pstate->s1 = prandom_seed_minimum(seed,   2U);
    pstate->s2 = prandom_seed_minimum(seed,   8U);
    pstate->s3 = prandom_seed_minimum(seed,  16U);
    pstate->s4 = prandom_seed_minimum(seed, 128U);
}

uint32_t prandom_state_value(struct prandom_state *pstate)
{
    pstate->s1 = TAUSWORTHE(pstate->s1,  6U, 13U, 4294967294U, 18U);
    pstate->s2 = TAUSWORTHE(pstate->s2,  2U, 27U, 4294967288U,  2U);
    pstate->s3 = TAUSWORTHE(pstate->s3, 13U, 21U, 4294967280U,  7U);
    pstate->s4 = TAUSWORTHE(pstate->s4,  3U, 12U, 4294967168U, 13U);
    return pstate->s1 ^ pstate->s2 ^ pstate->s3 ^ pstate->s4;
}
EXPORT_SYMBOL(prandom_state_value);

BYTEFILL_DECLARE_PREFIX(,
    prandom_state_bytes, uint32_t,
    prandom_state_value, (pstate),
    struct prandom_state *pstate
)
EXPORT_SYMBOL(prandom_state_bytes);

static void prandom_warmup(struct prandom_state *pstate)
{
    unsigned int count;
    for (count = 0; count < 10; ++count)
        prandom_state_value(pstate);
}

void prandom_state_seed(struct prandom_state *pstate, uint64_t seed)
{
    prandom_state_setup(pstate, seed);
    prandom_warmup(pstate);
}
EXPORT_SYMBOL(prandom_state_seed);

uint32_t prandom_value(void)
{
    struct prandom_state *pstate;
    uint32_t value;

    pstate = thiscpu_ptr(&prandom_state);

    preempt_disable();
    value = prandom_state_value(pstate);
    preempt_enable();

    return value;
}
EXPORT_SYMBOL(prandom_value);

void prandom_bytes(void *buff, size_t len)
{
    struct prandom_state *pstate;

    pstate = thiscpu_ptr(&prandom_state);

    preempt_disable();
    prandom_state_bytes(pstate, buff, len);
    preempt_enable();
}
EXPORT_SYMBOL(prandom_bytes);

void prandom_seed(uint64_t seed)
{
    struct prandom_state *pstate;
    unsigned int cpu;

    preempt_disable();
    cpu_for_each_possible(cpu) {
        pstate = percpu_ptr(cpu, &prandom_state);
        prandom_state_seed(pstate, seed);
    }
    preempt_enable();
}
EXPORT_SYMBOL(prandom_seed);
