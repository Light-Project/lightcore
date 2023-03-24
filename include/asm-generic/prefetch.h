/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PREFETCH_H_
#define _ASM_GENERIC_PREFETCH_H_

#include <asm/cache.h>

#ifndef PREFETCH_STRIDE
# define PREFETCH_STRIDE (CACHE_LINE_SIZE * 4)
#endif

#ifndef arch_prefetch
# define arch_prefetch(addr) __builtin_prefetch(addr)
#endif

#ifndef arch_prefetchw
# define arch_prefetchw(addr) __builtin_prefetch(addr, 1)
#endif

#endif /* _ASM_GENERIC_PREFETCH_H_ */
