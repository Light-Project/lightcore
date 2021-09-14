/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_BARRIER_H_
#define _ASM_ARM_BARRIER_H_

#ifndef __ASSEMBLY__

#if __ARM_ARCH__ >= 7
#define isb(op)     __asm__ __volatile__ ("isb" : : : "memory")
#define dsb(op)     __asm__ __volatile__ ("dsb" : : : "memory")
#define dmb(op)     __asm__ __volatile__ ("dmb" : : : "memory")
#elif __ARM_ARCH__ == 6
#define isb(op)     __asm__ __volatile__ ("mcr p15, 0, %0, c7, c5,  4" : : "r" (0) : "memory")
#define dsb(op)     __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define dmb(op)     __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 5" : : "r" (0) : "memory")
#else
#define isb(op)     __asm__ __volatile__ ("" : : : "memory")
#define dsb(op)     __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10,  4" : : "r" (0) : "memory")
#define dmb(op)     __asm__ __volatile__ ("" : : : "memory")
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

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_ARM_BARRIER_H_ */
