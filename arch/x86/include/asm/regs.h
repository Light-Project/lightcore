/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_REGS_H_ 
#define _ASM_X86_REGS_H_

#include <types.h>
#include <bits.h>

#ifndef __ASSEMBLY__

struct regs {
    union {
        struct {
            uint32_t edi;       /* 0x00: */
            uint32_t esi;       /* 0x04: */
            uint32_t ebp;       /* 0x08: */
            uint32_t _esp;      /* 0x0c: */
            uint32_t ebx;       /* 0x10: */
            uint32_t edx;       /* 0x14: */
            uint32_t ecx;       /* 0x18: */
            uint32_t eax;       /* 0x1c: */
            uint32_t _gsfs;     /* 0x20: */
            uint32_t _esds;     /* 0x24: */
            uint32_t _csss;     /* 0x28: */
            uint32_t eip;       /* 0x2c: */
            uint32_t eflags;    /* 0x30: */
            uint32_t esp;       /* 0x34: */
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
            uint16_t cs, ss;
            uint16_t ip, hip;
            uint16_t flags, hflags;
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
            uint8_t csl, csh, ssl, ssh;
            uint8_t ipl, iph, eip2, eip3;
            uint8_t lflags, hflags2, eflags2, eflags3;
            uint8_t spl, sph, esp2, esp3;
        };
    };
} __packed;

struct cr3{
    uint32_t RES0:3;    // Ignored
    uint32_t pwt:1;     // Page-level write-through
    uint32_t pcd:1;     // Page-level cache disable
    uint32_t RES1:7;    // Ignored
    uint32_t addr:20;   // Page directory used for linear-address translation
} __packed;

#define CR4_PSE     BIT(4)

static inline uint32_t cr0_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr0, %0\n\r"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr0_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr0\n\r"
        ::"Na"(val):
    );
}

static inline uint32_t cr2_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr2, %0\n\r"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr2_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr2\n\r"
        ::"Na"(val):
    );
}

static inline uint32_t cr3_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr3, %0\n\r"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr3_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr3\n\r"
        ::"Na"(val):
    );
}

static inline uint32_t cr4_get(void)
{
    uint32_t val;
    asm volatile(
        "movl   %%cr4, %0\n\r"
        :"=Na"(val)::
    );
    return val;
}

static inline void cr4_set(uint32_t val)
{
    asm volatile(
        "movl   %0, %%cr4\n\r"
        ::"Na"(val):
    );
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_REGS_H_ */
