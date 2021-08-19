/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_CSKY_CACHE_H_
#define _ASM_CSKY_CACHE_H_

#include <arch/csky/regs.h>

/* bytes per L1 cache line */
#ifdef CONFIG_CPU_CK610
    #define L1_CACHE_SHIFT 4
#endif /* CPU_CK610 */
#ifdef CONFIG_CPU_CK807
    #define L1_CACHE_SHIFT 5
#endif /* CPU_CK807 */
#ifdef CONFIG_CPU_CK810
    #define L1_CACHE_SHIFT 5
#endif /* CPU_CK810 */
#ifdef CONFIG_CPU_CK860
    #define L1_CACHE_SHIFT 6
#endif /* CPU_CK860 */

#define L1_CACHE_BYTES	(1 << L1_CACHE_SHIFT)

#define ARCH_DMA_MINALIGN	L1_CACHE_BYTES

#ifndef __ASSEMBLY__

void icache_inval_all(void);
void dcache_writeback_all(void);

#endif
#endif  /* __ASM_CSKY_CACHE_H */
