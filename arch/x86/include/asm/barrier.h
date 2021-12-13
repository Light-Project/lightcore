/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BARRIER_H_
#define _ASM_X86_BARRIER_H_

#include <asm/alternative.h>

#ifdef CONFIG_ARCH_X86
# define mb()   asm volatile("lock; addl $0,-4(%%esp)" :::"memory", "cc")
#else
# define mb()   asm volatile("mfence":::"memory")
# define rmb()  asm volatile("lfence":::"memory")
# define wmb()	asm volatile("sfence" ::: "memory")
#endif

#include <asm-generic/barrier.h>

#endif  /* _ASM_X86_BARRIER_H_ */
