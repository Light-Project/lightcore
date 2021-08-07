/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_PGALLOC_H_
#define _ASM_CSKY_PGALLOC_H_

#include <types.h>
#include <state.h>

void arch_page_setup(void);
state arch_page_map(size_t pa, size_t va, size_t size);

#endif  /* _ASM_CSKY_PGALLOC_H_ */
