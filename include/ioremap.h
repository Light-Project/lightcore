/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IOREMAP_H_
#define _IOREMAP_H_

#include <types.h>
#include <mm/gvm.h>

#ifndef CONFIG_MMU

static inline void __malloc *ioremap_node(phys_addr_t pa, size_t size, gvm_t flags)
{
    return (void *)pa;
}

static inline void __malloc *ioremap_wc(phys_addr_t pa, size_t size)
{
    return ioremap_node(pa, size, 0);
}

static inline void __malloc *ioremap_wt(phys_addr_t pa, size_t size)
{
    return ioremap_node(pa, size, 0);
}

static inline void __malloc *ioremap(phys_addr_t pa, size_t size)
{
    return ioremap_node(pa, size, 0);
}

static inline void __nonnull(1) iounmap(void *addr) {}
static inline void ioremap_init(void) {}

#else /* !CONFIG_MMU */

extern void __malloc *ioremap_node(phys_addr_t pa, size_t size, gvm_t flags);
extern void __malloc *ioremap(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_wc(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_wt(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_uc(phys_addr_t pa, size_t size);

extern void iounmap(void *addr);
extern void __init ioremap_init(void);

#endif  /* CONFIG_MMU */
#endif  /* _MM_IOREMAP_H_ */
