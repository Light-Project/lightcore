/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMEM_H_
#define _MM_VMEM_H_

#include <state.h>
#include <list.h>
#include <rbtree.h>
#include <mm/gfp.h>

/**
 * struct vmem_area - describe a virtual space.
 * @addr: addr of virtual space.
 * @size: size of virtual space.
 * @rb: address sorted rbtree.
 * @list: address sorted list.
 */
struct vmem_area {
    uintptr_t addr;
    size_t size;
    struct rb_node rb;
    struct list_head list;
};

#define rb_to_vma(node) \
    rb_entry(node, struct vmem_area, rb)

#define rb_to_vma_safe(node) \
    rb_entry_safe(node, struct vmem_area, rb)

#define list_to_vma(node) \
    rb_entry(node, struct vmem_area, list)

extern struct vmem_area *vmem_area_find(size_t addr);
extern state vmem_area_alloc_node(struct vmem_area *vma, size_t size, size_t align, size_t vstart, size_t vend);
extern state vmem_area_alloc_align(struct vmem_area *vma, size_t size, size_t align);
extern state vmem_area_alloc(struct vmem_area *vma, size_t size);
extern struct vmem_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp, size_t vstart, size_t vend);
extern struct vmem_area *vmem_alloc_align(size_t size, size_t align, gfp_t gfp);
extern struct vmem_area *vmem_alloc(size_t size);
extern void vmem_area_free(struct vmem_area *vma);
extern void vmem_free(struct vmem_area *vma);
extern void __init vmem_init(void);

#endif  /* _MM_VMEM_H_ */