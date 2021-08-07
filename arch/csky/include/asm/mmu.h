/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_MMU_H_
#define _ASM_MMU_H_

#include <asm/regs.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_CSKY_ABIV1

static inline int mmu_index_read(void)
{
    return cprcr("cpcr0");
}

static inline void mmu_index_write(int value)
{
    cpwcr("cpcr0", value);
}

static inline uint32_t mmu_entrylo0_read(void)
{
    return cprcr("cpcr2") << 6;
}

static inline uint32_t mmu_entrylo1_read(void)
{
    return cprcr("cpcr3") << 6;
}

static inline uint32_t mmu_entryhi_read(void)
{
    return cprcr("cpcr4");
}

static inline void mmu_entryhi_write(uint32_t value)
{
    cpwcr("cpcr4", value);
}

static inline void mmu_pagemask_write(uint32_t value)
{
    cpwcr("cpcr6", value);
}

static inline void mmu_mcir_write(uint32_t value)
{
    cpwcr("cpcr8", value);
}

static inline uint32_t mmu_msa0_read(void)
{
    return cprcr("cpcr30");
}

static inline void mmu_msa0_write(uint32_t value)
{
    cpwcr("cpcr30", value);
}

static inline uint32_t mmu_msa1_read(void)
{
    return cprcr("cpcr31");
}

static inline void mmu_msa1_write(uint32_t value)
{
    cpwcr("cpcr31", value);
}

#endif  /* CONFIG_CSKY_ABIV1 */

#endif /* __ASSEMBLY__ */
#endif /* __ASM_REGS_OPS_H */