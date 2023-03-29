/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_H_
#define _ASM_GENERIC_H_

#ifndef __ASSEMBLY__

#ifndef cache_flush_all
static inline void cache_flush_all(void)
{
    /* Nothing */
}
#endif

#ifndef dcache_flush_all
static inline void dcache_flush_all(void)
{
    /* Nothing */
}
#endif

#ifndef icache_flush_all
static inline void icache_flush_all(void)
{
    /* Nothing */
}
#endif

#ifndef cache_flush_vmap
static inline void cache_flush_vmap(void)
{
    /* Nothing */
}
#endif

#ifndef cache_flush_vunmap
static inline void cache_flush_vunmap(void)
{
    /* Nothing */
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* _ASM_GENERIC_H_ */
