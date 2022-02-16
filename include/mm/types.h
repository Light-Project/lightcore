/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_TYPES_H_
#define _MM_TYPES_H_

#include <list.h>
#include <rbtree.h>
#include <spinlock.h>
#include <refcount.h>
#include <asm/pgtable.h>

enum page_type {
    PAGE_UNINIT,
    PAGE_FREE,
    PAGE_USED,
    PAGE_SLOB,
};

/**
 * slob_page - slob page infrastructure
 * @list: slob page list
 * @head: slob page list head
 * @node: first slob node on slob page
 * @avail: free space size on slob pages
 */
struct slob_page {
    struct list_head list;
    struct list_head *head;
    struct slob_node *node;
    unsigned int avail;
};

/**
 * page - page infrastructure
 */
struct page {
    uint32_t type:4;
    uint32_t order:4;
#ifdef CONFIG_SPARCEMEM
    uint32_t sparce_nr:8;
#endif
    union {
        struct list_head free_list;
#ifdef CONFIG_MM_SLOB
        struct slob_page slob;
#endif
    };
};

#define slob_to_page(slob) \
    container_of(slob, struct page, slob)

/**
 * memory - describes the memory information of a task
 */
struct memory {
    spinlock_t pglock;
    pgd_t *pgdir;
    struct refcount users;

    size_t brk_start, brk_curr;
    size_t stack_start;
};

extern struct memory init_mm;

#endif  /* _MM_PAGE_DEF_H_ */
