/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_UNALIGNED_H_
#define _ASM_GENERIC_UNALIGNED_H_

#include <types.h>
#include <lightcore/asm/byteorder.h>

#ifndef unaligned_get
#define unaligned_get(type, ptr) ({         \
    const struct { type val; } __packed     \
    *_pptr = (typeof(_pptr))(ptr);          \
    _pptr->val;                             \
})
#endif

#ifndef unaligned_set
#define unaligned_set(type, ptr, value) ({  \
    struct { type val; } __packed           \
    *_pptr = (typeof(_pptr))(ptr);          \
    _pptr->val = (value);                   \
})
#endif

static inline uint16_t unaligned_get_le16(const void *p)
{
    return le16_to_cpu(unaligned_get(le16, p));
}

static inline uint32_t unaligned_get_le32(const void *p)
{
    return le32_to_cpu(unaligned_get(le32, p));
}

static inline uint64_t unaligned_get_le64(const void *p)
{
    return le64_to_cpu(unaligned_get(le64, p));
}

static inline void unaligned_set_le16(void *p, uint16_t val)
{
    unaligned_set(le16, p, cpu_to_le16(val));
}

static inline void unaligned_set_le32(void *p, uint32_t val)
{
    unaligned_set(le32, p, cpu_to_le32(val));
}

static inline void unaligned_set_le64(void *p, uint64_t val)
{
    unaligned_set(le64, p, cpu_to_le64(val));
}

static inline uint16_t unaligned_get_be16(const void *p)
{
    return be16_to_cpu(unaligned_get(be16, p));
}

static inline uint32_t unaligned_get_be32(const void *p)
{
    return be32_to_cpu(unaligned_get(be32, p));
}

static inline uint64_t unaligned_get_be64(const void *p)
{
    return be64_to_cpu(unaligned_get(be64, p));
}

static inline void unaligned_set_be16(void *p, uint16_t val)
{
    unaligned_set(be16, p, cpu_to_be16(val));
}

static inline void unaligned_set_be32(void *p, uint32_t val)
{
    unaligned_set(be32, p, cpu_to_be32(val));
}

static inline void unaligned_set_be64(void *p, uint64_t val)
{
    unaligned_set(be64, p, cpu_to_be64(val));
}

#endif  /* _ASM_GENERIC_UNALIGNED_H_ */
