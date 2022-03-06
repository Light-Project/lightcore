/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IDR_H_
#define _IDR_H_

#include <limits.h>
#include <list.h>
#include <rbtree.h>
#include <spinlock.h>

#define IDR_NONE 0

struct idr {
    spinlock_t lock;
    struct rb_root root;
    struct list_head list;
    unsigned long id_base;
    unsigned long id_next;
};

extern unsigned long idr_alloc_range(struct idr *idr, void *pdata, unsigned long min, unsigned long max, gfp_t flags);
extern unsigned long idr_alloc_cyclic_range(struct idr *idr, void *pdata, unsigned long min, unsigned long max, gfp_t flags);
extern void idr_free(struct idr *idr, unsigned long id);
extern void *idr_find(struct idr *idr, unsigned long id);
extern struct idr *idr_create(unsigned long base);
extern void idr_release(struct idr *idr);
extern void idr_delete(struct idr *idr);
extern void __init idr_init(void);

#define IDR_ALLOC_OP(name, function)                                                            \
static inline unsigned long                                                                     \
generic_idr_##name(struct idr *idr, void *pdata, gfp_t flags)                                   \
{                                                                                               \
    return function(idr, pdata, ULONG_MIN, ULONG_MAX, flags);                                   \
}                                                                                               \
                                                                                                \
static inline unsigned long                                                                     \
generic_idr_##name##_min(struct idr *idr, void *pdata, unsigned long min, gfp_t flags)          \
{                                                                                               \
    return function(idr, pdata, min, ULONG_MAX, flags);                                         \
}                                                                                               \
                                                                                                \
static inline unsigned long                                                                     \
generic_idr_##name##_max(struct idr *idr, void *pdata, unsigned long max, gfp_t flags)          \
{                                                                                               \
    return function(idr, pdata, ULONG_MIN, max, flags);                                         \
}

IDR_ALLOC_OP(alloc, idr_alloc_range)
#define idr_alloc(idr, pdata, flags)                    generic_idr_alloc(idr, pdata, flags)
#define idr_alloc_min(idr, pdata, min, flags)           generic_idr_alloc_min(idr, pdata, min, flags)
#define idr_alloc_max(idr, pdata, max, flags)           generic_idr_alloc_max(idr, pdata, max, flags)

IDR_ALLOC_OP(alloc_cyclic, idr_alloc_cyclic_range)
#define idr_alloc_cyclic(idr, pdata, flags)             generic_idr_alloc_cyclic(idr, pdata, flags)
#define idr_alloc_cyclic_min(idr, pdata, min, flags)    generic_idr_alloc_cyclic_min(idr, pdata, min, flags)
#define idr_alloc_cyclic_max(idr, pdata, max, flags)    generic_idr_alloc_cyclic_max(idr, pdata, max, flags)

#endif  /* _IDR_H_ */
