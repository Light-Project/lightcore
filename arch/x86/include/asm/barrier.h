/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BARRIER_H_
#define _ASM_X86_BARRIER_H_

#include <asm/alternative.h>

#define mb() asm volatile("lock; addl $0,-4(%%esp)" :::"memory", "cc")

#include <asm-generic/barrier.h>

#endif  /* _ASM_X86_BARRIER_H_ */
