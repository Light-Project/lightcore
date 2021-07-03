 

#ifndef _KMEMLEAK_H_
#define _KMEMLEAK_H_

#ifdef CONFIG_KMEMLEAK





#else 

static inline void kmemleak_init(void)
{
}

static inline void kmemleak_alloc(const void *ptr, size_t size, int min_count, gfp_t gfp)
{
}

static inline void kmemleak_vmalloc(const struct vm_struct *area, size_t size, gfp_t gfp)
{
}

static inline void kmemleak_free(const void *ptr)
{
}

#endif /* CONFIG_KMEMLEAK */

#endif /* _KMEMLEAK_H_ */
