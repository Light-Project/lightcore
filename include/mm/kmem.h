/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_KMEM_H_
#define _MM_KMEM_H_

#include <list.h>
#include <spinlock.h>
#include <mm/slob.h>
#include <mm/gfp.h>

#define heap_autograph 0x55aa00ff

#ifndef __ASSEMBLY__

void __malloc *kmalloc(size_t size, gfp_t);
#define kzalloc(size, gfp)  kmalloc(size, gfp | GFP_ZERO)

void __malloc *kcalloc (size_t nmemb, size_t size, gfp_t);
void __malloc *krealloc(void* mem, size_t size, gfp_t);
void kfree(void *mem);

void kmem_init(void);
void kmalloc_info(void);

#endif	/* __ASSEMBLY__ */
#endif /* _MM_KMEM_H_ */
