/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_IRQ_H_
#define _ASM_GENERIC_IRQ_H_

#include <types.h>
#include <limits.h>

#ifndef irqflags_t
#define irqflags_t size_t
#endif

#ifndef irqnr_t
#define irqnr_t size_t
#endif

#ifndef IRQ_NR_MAX
#define IRQ_NR_MAX ULONG_MAX
#endif

#endif  /* _ASM_GENERIC_IRQ_H_ */
