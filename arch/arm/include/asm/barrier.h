/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_BARRIER_H_
#define _ASM_ARM_BARRIER_H_

#ifndef __ASSEMBLY__

#if defined(CONFIG_ARCH_ARM_V7)
#define isb(op)     asm volatile("isb" : : : "memory")
#define dsb(op)     asm volatile("dsb" : : : "memory")
#define dmb(op)     asm volatile("dmb" : : : "memory")
#elif defined(CONFIG_ARCH_ARM_V6)
#define isb(op)     asm volatile("mcr p15, 0, %0, c7, c5,  4" : : "r" (0) : "memory")
#define dsb(op)     asm volatile("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define dmb(op)     asm volatile("mcr p15, 0, %0, c7, c10, 5" : : "r" (0) : "memory")
#else
#define isb(op)     asm volatile("" : : : "memory")
#define dsb(op)     asm volatile("mcr p15, 0, %0, c7, c10,  4" : : "r" (0) : "memory")
#define dmb(op)     asm volatile("" : : : "memory")
#endif

#ifdef CONFIG_SMP
#define mb()        dsb()
#define rmb()       dsb()
#define wmb()       dsb()
#endif

#define smp_mb()	dmb()
#define smp_rmb()	dmb()
#define smp_wmb()	dmb()

#include <asm-generic/barrier.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_ARM_BARRIER_H_ */
