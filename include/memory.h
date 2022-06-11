/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <sections.h>
#include <mm/page.h>
#include <mm/memmodel.h>

#define page_align(addr)        align_high(addr, PAGE_SIZE)
#define page_aligned(addr)      align_check(addr, PAGE_SIZE)
#define page_ptr_align(addr)    align_ptr_high(addr, PAGE_SIZE)
#define page_ptr_aligned(addr)  align_ptr_check(addr, PAGE_SIZE)

static inline void memory_get(struct memory *mem)
{
    refcount_inc(&mem->users);
}

static inline bool memory_put(struct memory *mem)
{
    return refcount_dec_test(&mem->users);
}

extern void __init mem_init(void);

#endif  /* _MEMORY_H_ */
