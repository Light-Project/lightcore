/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMAP_H_
#define _MM_VMAP_H_

#include <bits.h>

enum vmap_flags {
    VMAP_NOCACHE    = BIT(0),
};

extern state vmap_range(struct memory *mm, phys_addr_t phys, size_t addr, size_t size, enum vmap_flags flags);
extern state vmap_pages(struct memory *mm, struct page **page, unsigned long pnr, size_t addr, enum vmap_flags flags);

#endif  /* _MM_VMAP_H_ */
