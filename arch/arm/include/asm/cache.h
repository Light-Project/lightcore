/* SPDX-License-Identifier: GPL-2.0 */
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
        "mcr    p15, 0, ip, c7, c5, 0\n\t"
        :::"ip"
    );
}

static inline void dcache_inval(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c6, 0\n\t"
        :::"ip"
    );
}

static inline void dcache_drain_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c7, c10, 4\n\t"
        :::"ip"
    );
}

#endif  /* __ASSEMBLY__ */
#endif  /* __ASM_CSKY_CACHE_H */
