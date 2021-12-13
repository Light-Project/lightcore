/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_TYPES_H_
#define _MM_TYPES_H_

#include <types.h>
#include <list.h>
#include <rbtree.h>
#include <spinlock.h>
#include <asm/pgtable.h>

enum page_type {
    PAGE_UNINIT,
    PAGE_FREE,
    PAGE_SLOB,
};

struct slob_page {
    struct list_head list;  /* slob page list */
    struct slob_node* node; /* slob node list head of this page */
    unsigned int avail;     /* Free space on slob pages */
};

struct page {
    uint32_t type:4;
    uint32_t order:4;
    uint32_t free:1;
#ifdef CONFIG_SPARCEMEM
    uint32_t sparce_nr:8;
#endif
    union {
        struct {
            struct list_head free_list;
        };
#ifdef CONFIG_MM_SLOB
        struct slob_page slob;
#endif
    };
};

#define slob_to_page(slob) \
    container_of(slob, struct page, slob)

struct page_free {
    struct list_head list;  /* free page list */
    size_t free_nr;         /* free page number */
};

struct uvm_area {
    size_t addr;    /* virtual start address */
    size_t size;    /* virtual allocation size */
    struct list_head list;
};

struct memory {
    spinlock_t pglock;
    pgd_t *pgdir;
};

extern struct memory init_mm;

#endif  /* _MM_PAGE_DEF_H_ */
