/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_XTENSA_BARRIER_H_
#define _ASM_XTENSA_BARRIER_H_

#ifndef __ASSEMBLY__

#define mb()  ({ __asm__ __volatile__("memw" : : : "memory"); })
#define rmb() barrier()
#define wmb() mb()

#include <asm-generic/barrier.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_XTENSA_BARRIER_H_ */
