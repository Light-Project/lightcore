/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_CSKY_CACHE_H_
#define _ASM_CSKY_CACHE_H_


/* bytes per L1 cache line */
#ifdef CONFIG_CPU_CK610
    #define CACHE_LINE_SHIFT 4
#endif /* CPU_CK610 */
#ifdef CONFIG_CPU_CK807
    #define CACHE_LINE_SHIFT 5
#endif /* CPU_CK807 */
#ifdef CONFIG_CPU_CK810
    #define CACHE_LINE_SHIFT 5
#endif /* CPU_CK810 */
#ifdef CONFIG_CPU_CK860
    #define CACHE_LINE_SHIFT 6
#endif /* CPU_CK860 */

#define CACHE_LINE_SIZE	(1 << CACHE_LINE_SHIFT)

#define ARCH_DMA_MINALIGN	CACHE_LINE_SIZE

#ifndef __ASSEMBLY__

void dcache_wb_line(unsigned long start);

void icache_inv_range(unsigned long start, unsigned long end);
void icache_inv_all(void);
void local_icache_inv_all(void *priv);

void dcache_wb_range(unsigned long start, unsigned long end);
void dcache_wbinv_all(void);

void cache_wbinv_range(unsigned long start, unsigned long end);
void cache_wbinv_all(void);

void dma_wbinv_range(unsigned long start, unsigned long end);
void dma_inv_range(unsigned long start, unsigned long end);
void dma_wb_range(unsigned long start, unsigned long end);

#endif
#endif  /* __ASM_CSKY_CACHE_H */
