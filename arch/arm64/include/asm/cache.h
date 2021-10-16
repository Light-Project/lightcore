/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_CACHE_H_
#define _ASM_ARM_CACHE_H_

#include <types.h>
#include <kernel.h>
#include <asm/barrier.h>

#ifdef CONFIG_CPU_V7
#define CACHE_LINE_SHIFT 6
#else
#define CACHE_LINE_SHIFT 5
#endif

#define CACHE_LINE_SIZE  (1 << CACHE_LINE_SHIFT)
#define CACHE_LINE_MASK  (~(CACHE_LINE_SIZE - 1))

#ifndef __ASSEMBLY__

static inline void icache_enable(void)
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0\n"
        "orr    ip, ip, #(1<<12)\n"
        "mcr    p15, 0, ip, c1, c0, 0\n"
        :::"ip"
    );
}

static inline void icache_disable(void)
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0\n"
        "bic    ip, ip, #(1<<12)\n"
        "mcr    p15, 0, ip, c1, c0, 0\n"
        :::"ip"
    );
}

static inline void dcache_enable(void)
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0\n"
        "orr    ip, ip, #(1<<2)\n"
        "mcr    p15, 0, ip, c1, c0, 0\n"
        :::"ip"
    );
}

static inline void dcache_disable(void)
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0\n"
        "bic    ip, ip, #(1<<2)\n"
        "mcr    p15, 0, ip, c1, c0, 0\n"
        :::"ip"
    );
}

#ifndef CONFIG_SMP

static inline void icache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c5, 0\n"
        :::"ip"
    );
}

#else

static inline void icache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c1, 0\n"
        :::"ip"
    );
}

#endif

static inline void dcache_inval(uint32_t mva)
{
    asm volatile (
        "mcr    p15, 0, %0, c7, c6, 1\n"
        ::"r"(mva)
    );
}

static inline void dcache_writeback(uint32_t mva)
{
    asm volatile (
        "mcr    p15, 0, %0, c7, c14, 1\n"
        ::"r"(mva)
    );
}

void cache_inval_all(void);
void icache_inval(uint32_t mva);
void dcache_inval_all(void);
void dcache_writeback_all(void);
void wcache_drain_all(void);

static inline void dcache_inval_range(size_t start, size_t end)
{
    uint32_t mva;

    start = align_low(start, CACHE_LINE_SIZE);
    end = align_high(end, CACHE_LINE_SIZE);

    for (mva = start; mva < end; mva += CACHE_LINE_SIZE)
        dcache_inval(start);

    dsb();
}

static inline void dcache_writeback_range(size_t start, size_t end)
{
    uint32_t mva;

    start = align_high(start, CACHE_LINE_SIZE);
    end = align_high(end, CACHE_LINE_SIZE);

    for (mva = start; mva < end; mva += CACHE_LINE_SIZE)
        dcache_writeback(mva);

    dsb();
}

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_ARM_CACHE_H_ */
