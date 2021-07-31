/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_REGS_H_ 
#define _ASM_X86_REGS_H_

#include <types.h>
#include <arch/x86/regs.h>

#ifndef __ASSEMBLY__

struct regs {
    union {
        struct {
            uint32_t edi;
            uint32_t esi;
            uint32_t ebp;
            uint32_t _esp;
            uint32_t ebx;
            uint32_t edx;
            uint32_t ecx;
            uint32_t eax;
            uint32_t _gsfs;
            uint32_t _esds;
            uint32_t _ss;
            uint32_t esp;
        };
        struct {
            uint16_t di, hdi;
            uint16_t si, hsi;
            uint16_t bp, hbp;
            uint16_t _sp, _hsp;
            uint16_t bx, hbx;
            uint16_t dx, hdx;
            uint16_t cx, hcx;
            uint16_t ax, hax;
            uint16_t gs, fs;
            uint16_t es, ds;
            uint16_t ss, res0;
            uint16_t sp, hsp;
        };
        struct {
            uint8_t dil, dih, edi2, edi3;
            uint8_t sil, sih, esi2, esi3;
            uint8_t bpl, bph, ebp2, ebp3;
            uint8_t _spl, _sph, _esp2, _esp3;
            uint8_t bl, bh, ebx2, ebx3;
            uint8_t dl, dh, edx2, edx3;
            uint8_t cl, ch, ecx2, ecx3;
            uint8_t al, ah, eax2, eax3;
            uint8_t gsl, gsh, fsl, fsh;
            uint8_t esl, esh, dsl, dsh;
            uint8_t ssl, ssh, res1, res2;
            uint8_t spl, sph, esp2, esp3;
        };
    };
    uint32_t error;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __packed;

struct cr3{
    uint32_t RES0:3;    // Ignored
    uint32_t pwt:1;     // Page-level write-through
    uint32_t pcd:1;     // Page-level cache disable
    uint32_t RES1:7;    // Ignored
    uint32_t addr:20;   // Page directory used for linear-address translation
} __packed;

static inline uint32_t cr0_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr0, %0\n"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr0_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr0\n"
        ::"Na"(val):
    );
}

static inline uint32_t cr2_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr2, %0\n"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr2_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr2\n"
        ::"Na"(val):
    );
}

static inline uint32_t cr3_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr3, %0\n"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr3_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr3\n"
        ::"Na"(val):
    );
}

static inline uint32_t cr4_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr4, %0\n"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr4_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr4\n"
        ::"Na"(val):
    );
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_REGS_H_ */
