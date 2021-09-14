/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_CACHE_H_
#define _ASM_ARM_CACHE_H_

#ifdef CONFIG_
#define L1_CACHE_SHIFT 5
#elif CONFIG_
#define L1_CACHE_SHIFT 5
#else
#define L1_CACHE_SHIFT 5
#endif

#define L1_CACHE_BYTES	(1 << L1_CACHE_SHIFT)

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

static inline void cache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c7, 0\n"
        :::"ip"
    );
}

static inline void icache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c5, 0\n"
        :::"ip"
    );
}

static inline void dcache_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c6, 0\n"
        :::"ip"
    );
}

static inline void dcache_writeback_all(void)
{
    asm volatile (
    "1:  mrc    p15, 0, apsr_nzcv, c7, c14, 3\n"
        "bne    1b\n"
    );
}

static inline void wcache_drain_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c10, 4\n"
        :::"ip"
    );
}

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_ARM_CACHE_H_ */
