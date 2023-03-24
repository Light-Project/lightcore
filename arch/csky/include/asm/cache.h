/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_CACHE_H_
#define _ASM_CSKY_CACHE_H_

/* bytes per L1 cache line */
#if defined(CONFIG_CPU_CK610)
# define CACHE_LINE_SHIFT 4
#elif defined(CONFIG_CPU_CK807)
# define CACHE_LINE_SHIFT 5
#elif defined(CONFIG_CPU_CK810)
# define CACHE_LINE_SHIFT 5
#elif defined(CONFIG_CPU_CK860)
# define CACHE_LINE_SHIFT 6
#endif

#define CACHE_LINE_SIZE	(1 << CACHE_LINE_SHIFT)
#define CACHE_LINE_MASK (~(CACHE_LINE_SIZE - 1))

#ifndef __ASSEMBLY__

void icache_inval_all(void);
void dcache_writeback_all(void);

#endif /* __ASSEMBLY__ */
#endif /* __ASM_CSKY_CACHE_H */
