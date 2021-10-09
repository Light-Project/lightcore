/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_IOREMAP_H_
#define _MM_IOREMAP_H_

#include <types.h>

void *ioremap(phys_addr_t pa, size_t size);
void iounmap(void *addr);

#endif  /* _MM_IOREMAP_H_ */
