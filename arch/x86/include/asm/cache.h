/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_CACHE_H_
#define _ASM_X86_CACHE_H_

#define CACHE_LINE_SHIFT CONFIG_CPU_CACHE_SHIFT
#define CACHE_LINE_SIZE	(1 << CACHE_LINE_SHIFT)
#define CACHE_LINE_MASK (~(CACHE_LINE_SIZE - 1))

#endif  /* _ASM_X86_CACHE_H_ */
