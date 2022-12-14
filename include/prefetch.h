/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PREFETCH_H_
#define _PREFETCH_H_

#include <asm/prefetch.h>

/*
 * prefetch(addr) - prefetches the cacheline at @addr for read.
 * prefetchw(addr) - prefetches the cacheline at @addr for write.
 */

#ifdef CONFIG_ARCH_HAS_PREFETCH
# define prefetch(addr) arch_prefetch(addr)
#else
# define prefetch(addr) (void)(addr)
#endif

#ifdef CONFIG_ARCH_HAS_PREFETCHW
# define prefetchw(addr) arch_prefetchw(addr)
#else
# define prefetchw(addr) (void)(addr)
#endif

static inline void prefetch_range(const void *addr, size_t len)
{
#ifdef CONFIG_ARCH_HAS_PREFETCH
    const void *end = addr + len;
    const void *walk;

    for (walk = addr; walk < end; walk += PREFETCH_STRIDE)
        prefetch(walk);
#endif
}

static inline void prefetchw_range(const void *addr, size_t len)
{
#ifdef CONFIG_ARCH_HAS_PREFETCH
    const void *end = addr + len;
    const void *walk;

    for (walk = addr; walk < end; walk += PREFETCH_STRIDE)
        prefetchw(walk);
#endif
}

#endif  /* _PREFETCH_H_ */
