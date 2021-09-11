/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_CSKY_MMU_H_
#define _ASM_CSKY_MMU_H_

#include <asm/regs.h>

static inline int mmu_index_get(void)
{
    return cprcr("cpcr0");
}

static inline void mmu_index_set(int value)
{
    cpwcr("cpcr0", value);
}

static inline uint32_t mmu_entrylo0_get(void)
{
    return cprcr("cpcr2") << 6;
}

static inline uint32_t mmu_entrylo1_get(void)
{
    return cprcr("cpcr3") << 6;
}

static inline uint32_t mmu_entryhi_get(void)
{
    return cprcr("cpcr4");
}

static inline void mmu_entryhi_set(uint32_t value)
{
    cpwcr("cpcr4", value);
}

static inline void mmu_pagemask_set(uint32_t value)
{
    cpwcr("cpcr6", value);
}

static inline void mmu_mcir_set(uint32_t value)
{
    cpwcr("cpcr8", value);
}

static inline uint32_t mmu_pgd_get(void)
{
    return cprcr("cpcr29");
}

static inline void mmu_pgd_set(uint32_t value)
{
    cpwcr("cpcr29", value);
}

static inline uint32_t mmu_msa0_get(void)
{
    return cprcr("cpcr30");
}

static inline void mmu_msa0_set(uint32_t value)
{
    cpwcr("cpcr30", value);
}

static inline uint32_t mmu_msa1_get(void)
{
    return cprcr("cpcr31");
}

static inline void mmu_msa1_set(uint32_t value)
{
    cpwcr("cpcr31", value);
}

#endif  /* _ASM_CSKY_MMU_H_ */
