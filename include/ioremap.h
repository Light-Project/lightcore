/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IOREMAP_H_
#define _IOREMAP_H_

#include <types.h>
#include <mm/gvm.h>

extern void __malloc *ioremap_node(phys_addr_t pa, size_t size, gvm_t flags);
extern void __malloc *ioremap(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_wc(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_uc(phys_addr_t pa, size_t size);
extern void __malloc *ioremap_wt(phys_addr_t pa, size_t size);

extern void iounmap(void *addr);
extern void __init ioremap_init(void);

#endif  /* _MM_IOREMAP_H_ */
