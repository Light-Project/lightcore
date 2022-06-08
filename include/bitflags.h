/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITFLAGS_H_
#define _BITFLAGS_H_

#include <bitops.h>

#define GENERIC_STRUCT_BITOPS(name, type, member)               \
static __always_inline void                                     \
name##_##member##_set(type *ptr, unsigned int bit)              \
{                                                               \
    bit_set(&ptr->member, bit);                                 \
}                                                               \
                                                                \
static __always_inline void                                     \
name##_##member##_clr(type *ptr, unsigned int bit)              \
{                                                               \
    bit_clr(&ptr->member, bit);                                 \
}                                                               \
                                                                \
static __always_inline bool                                     \
name##_##member##_test(type *ptr, unsigned int bit)             \
{                                                               \
    return bit_test(&ptr->member, bit);                         \
}

#define GENERIC_STRUCT_FLAG(name, type, member, flag, index)    \
static __always_inline void name##_set_##flag(type *ptr)        \
{                                                               \
    name##_##member##_set(ptr, index);                          \
}                                                               \
                                                                \
static __always_inline void name##_clr_##flag(type *ptr)        \
{                                                               \
    name##_##member##_clr(ptr, index);                          \
}                                                               \
                                                                \
static __always_inline bool name##_test_##flag(type *ptr)       \
{                                                               \
    return name##_##member##_test(ptr, index);                  \
}

#endif  /* _BITFLAGS_H_ */
