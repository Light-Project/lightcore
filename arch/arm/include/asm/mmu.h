/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_MMU_H_
#define _ASM_ARM_MMU_H_

static inline void mmu_enable()
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0 \n\t"
        "orr    ip, ip, #0x1 \n\t"
        "mcr    p15, 0, ip, c1, c0, 0 \n\t"
        :::"ip"
    );
}

static inline void mmu_disable()
{
    asm volatile (
        "mrc    p15, 0, ip, c1, c0, 0 \n\t"
        "bic    ip, ip, #0x1 \n\t"
        "mcr    p15, 0, ip, c1, c0, 0 \n\t"
        :::"ip"
    );
}

#endif  /* _ASM_ARM_MMU_H_ */
