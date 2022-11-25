/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITFLAGS_H_
#define _BITFLAGS_H_

#include <bitops.h>

#define GENERIC_STRUCT_BITOPS(name, type, member, safe)         \
static __always_inline void                                     \
name##_##member##_clr(type *ptr, unsigned int bit)              \
{                                                               \
    if (!safe || ptr)                                           \
        bit_clr(&ptr->member, bit);                             \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_set(type *ptr, unsigned int bit)              \
{                                                               \
    if (!safe || ptr)                                           \
        bit_set(&ptr->member, bit);                             \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_flip(type *ptr, unsigned int bit)             \
{                                                               \
    if (!safe || ptr)                                           \
        bit_flip(&ptr->member, bit);                            \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_test(type *ptr, unsigned int bit)             \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_test(&ptr->member, bit);                     \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_test_clr(type *ptr, unsigned int bit)         \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_test_clr(&ptr->member, bit);                 \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_test_set(type *ptr, unsigned int bit)         \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_test_set(&ptr->member, bit);                 \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_test_flip(type *ptr, unsigned int bit)        \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_test_flip(&ptr->member, bit);                \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_atomic_clr(type *ptr, unsigned int bit)       \
{                                                               \
    if (!safe || ptr)                                           \
        bit_atomic_clr(&ptr->member, bit);                      \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_atomic_set(type *ptr, unsigned int bit)       \
{                                                               \
    if (!safe || ptr)                                           \
        bit_atomic_set(&ptr->member, bit);                      \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_atomic_flip(type *ptr, unsigned int bit)      \
{                                                               \
    if (!safe || ptr)                                           \
        bit_atomic_flip(&ptr->member, bit);                     \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_atomic_test(type *ptr, unsigned int bit)      \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_atomic_test(&ptr->member, bit);              \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_atomic_test_clr(type *ptr, unsigned int bit)  \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_atomic_test_clr(&ptr->member, bit);          \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_atomic_test_set(type *ptr, unsigned int bit)  \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_atomic_test_set(&ptr->member, bit);          \
    return false;                                               \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_atomic_test_flip(type *ptr, unsigned int bit) \
{                                                               \
    if (!safe || ptr)                                           \
        return bit_atomic_test_flip(&ptr->member, bit);         \
    return false;                                               \
}

#define GENERIC_STRUCT_FLAG(name, type, member, flag, index)    \
static __always_inline void                                     \
name##_clr_##flag(type *ptr)                                    \
{                                                               \
    name##_##member##_clr(ptr, index);                          \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_set_##flag(type *ptr)                                    \
{                                                               \
    name##_##member##_set(ptr, index);                          \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_test_##flag(type *ptr)                                   \
{                                                               \
    return name##_##member##_test(ptr, index);                  \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_test_clr_##flag(type *ptr)                               \
{                                                               \
    return name##_##member##_test_clr(ptr, index);              \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_test_set_##flag(type *ptr)                               \
{                                                               \
    return name##_##member##_test_set(ptr, index);              \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_test_flip_##flag(type *ptr)                              \
{                                                               \
    return name##_##member##_test_flip(ptr, index);             \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_atomic_clr_##flag(type *ptr)                             \
{                                                               \
    name##_##member##_atomic_clr(ptr, index);                   \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_atomic_set_##flag(type *ptr)                             \
{                                                               \
    name##_##member##_atomic_set(ptr, index);                   \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_atomic_test_##flag(type *ptr)                            \
{                                                               \
    return name##_##member##_atomic_test(ptr, index);           \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_atomic_test_clr_##flag(type *ptr)                        \
{                                                               \
    return name##_##member##_atomic_test_clr(ptr, index);       \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_atomic_test_set_##flag(type *ptr)                        \
{                                                               \
    return name##_##member##_atomic_test_set(ptr, index);       \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_atomic_test_flip_##flag(type *ptr)                       \
{                                                               \
    return name##_##member##_atomic_test_flip(ptr, index);      \
}

#endif  /* _BITFLAGS_H_ */
