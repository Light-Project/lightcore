/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMAP_H_
#define _MM_VMAP_H_

#include <bits.h>
#include <mm/types.h>
#include <mm/gvm.h>

extern state vmap_range(struct memory *mm, phys_addr_t phys, size_t addr, size_t size, gvm_t flags, unsigned int pgshift);
extern state vmap_pages(struct memory *mm, struct page **page, size_t addr, size_t size, gvm_t flags, unsigned int pgshift);
extern void vunmap_range(struct memory *mm, size_t addr, size_t size);

#endif  /* _MM_VMAP_H_ */
