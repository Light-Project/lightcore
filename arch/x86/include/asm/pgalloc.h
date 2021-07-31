/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PGALLOC_H_
#define _ASM_X86_PGALLOC_H_

#include <state.h>

state arch_page_map(size_t pa, size_t va, size_t size);

#endif  /* _ASM_X86_PGALLOC_H_ */
