/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ASM_X86_REGS_H_
#define _ASM_X86_REGS_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>
#include <asm/asm.h>
#include <asm/extable.h>
#include <arch/x86/regs.h>
#include <arch/x86/cpuid.h>
#include <arch/x86/msr.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_ARCH_X86_64

struct regs {
    /* segment registers */
    uint64_t ds;
    uint64_t es;
    uint64_t ss;
    uint64_t fs;
    uint64_t gs;

    /* generic registers */
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t bp;
    uint64_t bx;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t ax;
    uint64_t cx;
    uint64_t dx;
    uint64_t si;
    uint64_t di;
    uint64_t sp;

    /* interrupt stack */
    uint64_t error;
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t usp;
    uint64_t uss;
} __packed;

#else  /* !CONFIG_ARCH_X86_64 */

struct regs {
    /* generic registers */
    uint32_t di;
    uint32_t si;
    uint32_t bp;
    uint32_t sp;
    uint32_t bx;
    uint32_t dx;
    uint32_t cx;
    uint32_t ax;

    /* segment registers */
    uint32_t ds;
    uint32_t es;
    uint32_t ss;
    uint32_t fs;
    uint32_t gs;

    /* interrupt stack */
    uint32_t error;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t usp;
    uint32_t uss;
} __packed;

#endif  /* CONFIG_ARCH_X86_64 */

static inline unsigned long eflags_get(void)
{
    unsigned long flags;

    /*
     * "=rm" is safe here, because "pop" adjusts the stack before
     * it evaluates its effective address -- this is part of the
     * documented behavior of the "pop" instruction.
     */

    asm volatile (
        "pushf      \n"
        "pop    %0  \n"
        : "=rm" (flags)
        :: "memory"
    );

    return flags;
}

static inline void eflags_set(unsigned long flags)
{
    asm volatile (
        "push   %0 \n"
        "popf      \n"
        :: "rm" (flags)
        : "memory"
    );
}

static inline void *gdt_get(void)
{
    void *addr;

    asm volatile (
        "sgdt (%0)\n"
        : "=r"(addr)
    );

    return addr;
}

static inline void gdt_set(void *addr)
{
    asm volatile (
        "lgdt (%0)\n"
        :: "r"(addr)
    );
}

static inline void *idt_get(void)
{
    void *addr;

    asm volatile (
        "sidt (%0)\n"
        : "=r"(addr)
    );

    return addr;
}

static inline void idt_set(void *addr)
{
    asm volatile (
        "lidt (%0)\n"
        :: "r"(addr)
    );
}

static inline unsigned long tss_get(void)
{
    unsigned long seg;

    asm volatile (
        "str %0\n"
        : "=r"(seg)
    );

    return seg;
}

static inline void tss_set(unsigned long seg)
{
    asm volatile (
        "ltr %0\n"
        :: "r"(seg)
    );
}

static inline uint64_t tsc_get(void)
{
    DECLARE_ARGS(val, low, high);

    asm volatile (
        "rdtsc\n"
        : EAX_EDX_RET(val, low, high)
    );

    return EAX_EDX_VAL(val, low, high);
}

static inline uint64_t msr_get(unsigned int msr)
{
    DECLARE_ARGS(val, low, high);

    asm volatile (
        "1: rdmsr   \n"
        "2:         \n"
        EXTABLE_ASM(1b, 2b, EXTABLE_TYPE_RDMSR)
        : EAX_EDX_RET(val, low, high)
        : "c" (msr)
    );

    return EAX_EDX_VAL(val, low, high);
}

static inline void msr_set(unsigned int msr, uint64_t val)
{
    uint32_t high, low;

    low  = val & UINT32_MAX;
    high = val >> 32;

    asm volatile (
        "1: wrmsr   \n"
        "2:         \n"
        EXTABLE_ASM(1b, 2b, EXTABLE_TYPE_WRMSR)
        :: "c"(msr), "a"(low), "d"(high)
        : "memory"
    );
}

static inline bool rand_get(unsigned long *val)
{
    unsigned int retry = 10;
    bool success;

    while (retry--) {
        asm volatile (
            "rdrand %[out]\n" CC_SET(c)
            : CC_OUT(c) (success), [out] "=r" (*val)
        );
        if (success)
            return true;
    }

    return false;
}

static inline bool seed_get(unsigned long *val)
{
    bool success;

    asm volatile (
        "rdseed %[out]\n" CC_SET(c)
        : CC_OUT(c) (success), [out] "=r" (*val)
    );

    return success;
}

static inline void cpuid_asm(uint32_t *eax, uint32_t *ebx,
                             uint32_t *ecx, uint32_t *edx)
{
    asm volatile (
        "cpuid"
        : "=a"(*eax), "=b"(*ebx),
          "=c"(*ecx), "=d"(*edx)
        :  "0"(*eax),  "2"(*ecx)
    );
}

static inline void cpuid(const uint32_t op,
                         uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx)
{
    *eax = op;
    *ecx = 0;
    cpuid_asm(eax, ebx, ecx, edx);
}

static inline void cpuid_count(const uint32_t op,
                               const uint32_t count,
                               uint32_t *eax, uint32_t *ebx,
                               uint32_t *ecx, uint32_t *edx)
{
    *eax = op;
    *ecx = count;
    cpuid_asm(eax, ebx, ecx, edx);
}

static inline bool cpuid_support(const uint64_t code)
{
    uint32_t eax, ebx, ecx, edx;

    if (X86_CPUID_REG_GET(code) >= X86_CPUID_LNX)
        return false;

    cpuid(
        X86_CPUID_OP_GET(code),
        &eax, &ebx, &ecx, &edx
    );

    switch (X86_CPUID_REG_GET(code)) {
        case X86_CPUID_EDX:
            eax = edx;
            break;
        case X86_CPUID_ECX:
            eax = ecx;
            break;
        case X86_CPUID_EBX:
            eax = ebx;
            break;
        case X86_CPUID_EAX:
            break;
        default: /* BUG */
            return false;
    }

    return !!(BIT(X86_CPUID_BIT_GET(code)) & eax);
}

#define CPUID_REG_OP(reg)               \
static __always_inline uint32_t         \
generic_cpuid_##reg(uint32_t op)        \
{                                       \
    uint32_t eax, ebx, ecx, edx;        \
    cpuid(op, &eax, &ebx, &ecx, &edx);  \
    return reg;                         \
}

CPUID_REG_OP(eax)
CPUID_REG_OP(ebx)
CPUID_REG_OP(ecx)
CPUID_REG_OP(edx)

#define cpuid_eax generic_cpuid_eax
#define cpuid_ebx generic_cpuid_ebx
#define cpuid_ecx generic_cpuid_ecx
#define cpuid_edx generic_cpuid_edx

#define REG_GET_OP(type, reg)           \
static __always_inline type             \
generic_get_##reg(void)                 \
{                                       \
    type val;                           \
                                        \
    asm volatile (                      \
        _ASM_MOV " %%"#reg", %0\n"      \
        :"=Na"(val)                     \
    );                                  \
                                        \
    return val;                         \
}

#define REG_SET_OP(type, reg)           \
static __always_inline void             \
generic_set_##reg(type value)           \
{                                       \
    asm volatile (                      \
        _ASM_MOV " %0, %%"#reg"\n"      \
        ::"Na"(value)                   \
    );                                  \
}

REG_GET_OP(size_t, cs)
REG_GET_OP(size_t, ds)
REG_GET_OP(size_t, ss)
REG_GET_OP(size_t, es)
REG_GET_OP(size_t, gs)
REG_GET_OP(size_t, fs)
REG_SET_OP(size_t, cs)
REG_SET_OP(size_t, ds)
REG_SET_OP(size_t, ss)
REG_SET_OP(size_t, es)
REG_SET_OP(size_t, gs)
REG_SET_OP(size_t, fs)

REG_GET_OP(size_t, cr0)
REG_GET_OP(size_t, cr2)
REG_GET_OP(size_t, cr3)
REG_GET_OP(size_t, cr4)
REG_SET_OP(size_t, cr0)
REG_SET_OP(size_t, cr2)
REG_SET_OP(size_t, cr3)
REG_SET_OP(size_t, cr4)

REG_GET_OP(size_t, db0)
REG_GET_OP(size_t, db1)
REG_GET_OP(size_t, db2)
REG_GET_OP(size_t, db3)
REG_GET_OP(size_t, db6)
REG_GET_OP(size_t, db7)
REG_SET_OP(size_t, db0)
REG_SET_OP(size_t, db1)
REG_SET_OP(size_t, db2)
REG_SET_OP(size_t, db3)
REG_SET_OP(size_t, db6)
REG_SET_OP(size_t, db7)

#define ds_get generic_get_ds
#define ss_get generic_get_ss
#define es_get generic_get_es
#define gs_get generic_get_gs
#define fs_get generic_get_fs
#define ds_set generic_set_ds
#define ss_set generic_set_ss
#define es_set generic_set_es
#define gs_set generic_set_gs
#define fs_set generic_set_fs

#define cr0_get generic_get_cr0
#define cr2_get generic_get_cr2
#define cr3_get generic_get_cr3
#define cr4_get generic_get_cr4
#define cr0_set generic_set_cr0
#define cr2_set generic_set_cr2
#define cr3_set generic_set_cr3
#define cr4_set generic_set_cr4

#define dr0_get generic_get_db0
#define dr1_get generic_get_db1
#define dr2_get generic_get_db2
#define dr3_get generic_get_db3
#define dr6_get generic_get_db6
#define dr7_get generic_get_db7
#define dr0_set generic_set_db0
#define dr1_set generic_set_db1
#define dr2_set generic_set_db2
#define dr3_set generic_set_db3
#define dr6_set generic_set_db6
#define dr7_set generic_set_db7

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_REGS_H_ */
