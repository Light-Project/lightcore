/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_IOREMAP_H_
#define _MM_IOREMAP_H_

#include <types.h>

#ifndef CONFIG_MMU

static inline void __malloc *ioremap(phys_addr_t pa, size_t size)
{
    return (void *)pa;
}

static inline void __nonnull(1) iounmap(void *addr) {}
static inline void ioremap_init(void) {}

#else /* !CONFIG_MMU */

extern void __malloc *ioremap(phys_addr_t pa, size_t size);
extern void __nonnull(1) iounmap(void *addr);
extern void __init ioremap_init(void);

#endif  /* CONFIG_MMU */
#endif  /* _MM_IOREMAP_H_ */
