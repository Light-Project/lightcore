/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_UNALIGNED_H_
#define _ASM_GENERIC_UNALIGNED_H_

#include <types.h>
#include <lightcore/asm/byteorder.h>

#ifndef unaligned_get_type
#define unaligned_get_type(type, ptr) ({        \
    const struct { type val; } __packed         \
    *_pptr = (typeof(_pptr))(ptr);              \
    _pptr->val;                                 \
})
#endif

#ifndef unaligned_set_type
#define unaligned_set_type(type, ptr, value) ({ \
    struct { type val; } __packed               \
    *_pptr = (typeof(_pptr))(ptr);              \
    _pptr->val = (value);                       \
})
#endif

#ifndef unaligned_get
#define unaligned_get(ptr) unaligned_get_type(typeof(*(ptr)), (ptr))
#endif

#ifndef unaligned_set
#define unaligned_set(ptr, val) unaligned_set_type(typeof(*(ptr)), (ptr), (val))
#endif

static inline unsigned int unaligned_get_uint(const void *p)
{
    return unaligned_get_type(unsigned int, p);
}

static inline unsigned long unaligned_get_ulong(const void *p)
{
    return unaligned_get_type(unsigned long, p);
}

static inline void unaligned_set_uint(void *p, unsigned int val)
{
    unaligned_set_type(unsigned int, p, val);
}

static inline void unaligned_set_ulong(void *p, unsigned long val)
{
    unaligned_set_type(unsigned long, p, val);
}

static inline uint16_t unaligned_get_u16(const void *p)
{
    return unaligned_get_type(uint16_t, p);
}

static inline uint32_t unaligned_get_u32(const void *p)
{
    return unaligned_get_type(uint32_t, p);
}

static inline uint64_t unaligned_get_u64(const void *p)
{
    return unaligned_get_type(uint64_t, p);
}

static inline void unaligned_set_u16(void *p, uint16_t val)
{
    unaligned_set_type(uint16_t, p, val);
}

static inline void unaligned_set_u32(void *p, uint32_t val)
{
    unaligned_set_type(uint32_t, p, val);
}

static inline void unaligned_set_u64(void *p, uint64_t val)
{
    unaligned_set_type(uint64_t, p, val);
}

static inline uint16_t unaligned_get_le16(const void *p)
{
    return le16_to_cpu(unaligned_get_type(le16, p));
}

static inline uint32_t unaligned_get_le32(const void *p)
{
    return le32_to_cpu(unaligned_get_type(le32, p));
}

static inline uint64_t unaligned_get_le64(const void *p)
{
    return le64_to_cpu(unaligned_get_type(le64, p));
}

static inline void unaligned_set_le16(void *p, uint16_t val)
{
    unaligned_set_type(le16, p, cpu_to_le16(val));
}

static inline void unaligned_set_le32(void *p, uint32_t val)
{
    unaligned_set_type(le32, p, cpu_to_le32(val));
}

static inline void unaligned_set_le64(void *p, uint64_t val)
{
    unaligned_set_type(le64, p, cpu_to_le64(val));
}

static inline uint16_t unaligned_get_be16(const void *p)
{
    return be16_to_cpu(unaligned_get_type(be16, p));
}

static inline uint32_t unaligned_get_be32(const void *p)
{
    return be32_to_cpu(unaligned_get_type(be32, p));
}

static inline uint64_t unaligned_get_be64(const void *p)
{
    return be64_to_cpu(unaligned_get_type(be64, p));
}

static inline void unaligned_set_be16(void *p, uint16_t val)
{
    unaligned_set_type(be16, p, cpu_to_be16(val));
}

static inline void unaligned_set_be32(void *p, uint32_t val)
{
    unaligned_set_type(be32, p, cpu_to_be32(val));
}

static inline void unaligned_set_be64(void *p, uint64_t val)
{
    unaligned_set_type(be64, p, cpu_to_be64(val));
}

#endif /* _ASM_GENERIC_UNALIGNED_H_ */
