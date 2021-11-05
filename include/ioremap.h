/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_IOREMAP_H_
#define _MM_IOREMAP_H_

#include <types.h>

extern void __malloc *ioremap(phys_addr_t pa, size_t size);
extern void __nonnull(1) iounmap(void *addr);
extern void __init ioremap_init(void);

#endif  /* _MM_IOREMAP_H_ */
