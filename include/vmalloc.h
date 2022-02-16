/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _VMALLOC_H_
#define _VMALLOC_H_

#include <types.h>
#include <mm/gvm.h>

struct page;

extern void __malloc *vmalloc_pages(struct page **pages, unsigned int count, gvm_t flags);
extern void __malloc *vmalloc_numa(size_t size, size_t align, gfp_t flags, int numa);
extern void __malloc *vmalloc(size_t size);
extern void __malloc *vzalloc(size_t size);
extern void vfree_pages(void *block);
extern void vfree(void *block);
extern void __init vmalloc_init(void);

#endif  /* _MM_VMAP_H_ */
