/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _OVERFLOW_H_
#define _OVERFLOW_H_

#define overflow_check_add(a, b, d) ({          \
    typeof(a) ___a = (a);                       \
    typeof(b) ___b = (b);                       \
    typeof(d) ___d = (d);                       \
    (void) (&___a == &___b);                    \
    (void) (&___a == ___d);                     \
    __builtin_add_overflow(___a, ___b, ___d);   \
})

#define overflow_check_sub(a, b, d) ({          \
    typeof(a) ___a = (a);                       \
    typeof(b) ___b = (b);                       \
    typeof(d) ___d = (d);                       \
    (void) (&___a == &___b);                    \
    (void) (&___a == ___d);                     \
    __builtin_sub_overflow(___a, ___b, ___d);   \
})

#define overflow_check_mul(a, b, d) ({          \
    typeof(a) ___a = (a);                       \
    typeof(b) ___b = (b);                       \
    typeof(d) ___d = (d);                       \
    (void) (&___a == &___b);                    \
    (void) (&___a == ___d);                     \
    __builtin_mul_overflow(___a, ___b, ___d);   \
})

#define overflow_add_type(type, a, b) ({        \
    type __a = (type)(a);                       \
    type __b = (type)(b);                       \
    type __d;                                   \
    overflow_check_add(__a, __b, &__d)          \
    ? (type)~0ULL : __d;                        \
})

#define overflow_sub_type(type, a, b) ({        \
    type __a = (type)(a);                       \
    type __b = (type)(b);                       \
    type __d;                                   \
    overflow_check_sub(__a, __b, &__d)          \
    ? (type)~0ULL : __d;                        \
})

#define overflow_mul_type(type, a, b) ({        \
    type __a = (type)(a);                       \
    type __b = (type)(b);                       \
    type __d;                                   \
    overflow_check_mul(__a, __b, &__d)          \
    ? (type)~0ULL : __d;                        \
})

#define overflow_add(a, b) overflow_add_type(typeof(a), a, b)
#define overflow_sub(a, b) overflow_sub_type(typeof(a), a, b)
#define overflow_mul(a, b) overflow_mul_type(typeof(a), a, b)

#define GENERIC_OVERFLOW_OPS(name, type)                \
static inline type overflow_add_##name(type a, type b)  \
{                                                       \
    return overflow_add_type(type, a, b);               \
}                                                       \
static inline type overflow_sub_##name(type a, type b)  \
{                                                       \
    return overflow_sub_type(type, a, b);               \
}                                                       \
static inline type overflow_mul_##name(type a, type b)  \
{                                                       \
    return overflow_mul_type(type, a, b);               \
}

GENERIC_OVERFLOW_OPS(s8, int8_t)
GENERIC_OVERFLOW_OPS(s16, int16_t)
GENERIC_OVERFLOW_OPS(s32, int32_t)
GENERIC_OVERFLOW_OPS(s64, int64_t)
GENERIC_OVERFLOW_OPS(u8, uint8_t)
GENERIC_OVERFLOW_OPS(u16, uint16_t)
GENERIC_OVERFLOW_OPS(u32, uint32_t)
GENERIC_OVERFLOW_OPS(u64, uint64_t)

#endif  /* _OVERFLOW_H_ */
