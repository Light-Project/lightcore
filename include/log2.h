/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOG2_H_
#define _LOG2_H_

#include <bitops.h>

/**
 * ilog2_const - log base 2 of long long constant unsigned value.
 * @val: constant value to log base 2.
 */
#define ilog2_const(val) (          \
__builtin_constant_p(val) ? (       \
    (val) < 2 ? 0 :                 \
    (val) & (1ULL << 63) ? 63 :	    \
    (val) & (1ULL << 62) ? 62 :	    \
    (val) & (1ULL << 61) ? 61 :	    \
    (val) & (1ULL << 60) ? 60 :	    \
    (val) & (1ULL << 59) ? 59 :	    \
    (val) & (1ULL << 58) ? 58 :	    \
    (val) & (1ULL << 57) ? 57 :	    \
    (val) & (1ULL << 56) ? 56 :	    \
    (val) & (1ULL << 55) ? 55 :	    \
    (val) & (1ULL << 54) ? 54 :	    \
    (val) & (1ULL << 53) ? 53 :	    \
    (val) & (1ULL << 52) ? 52 :	    \
    (val) & (1ULL << 51) ? 51 :	    \
    (val) & (1ULL << 50) ? 50 :	    \
    (val) & (1ULL << 49) ? 49 :	    \
    (val) & (1ULL << 48) ? 48 :	    \
    (val) & (1ULL << 47) ? 47 :	    \
    (val) & (1ULL << 46) ? 46 :	    \
    (val) & (1ULL << 45) ? 45 :	    \
    (val) & (1ULL << 44) ? 44 :	    \
    (val) & (1ULL << 43) ? 43 :	    \
    (val) & (1ULL << 42) ? 42 :	    \
    (val) & (1ULL << 41) ? 41 :	    \
    (val) & (1ULL << 40) ? 40 :	    \
    (val) & (1ULL << 39) ? 39 :	    \
    (val) & (1ULL << 38) ? 38 :	    \
    (val) & (1ULL << 37) ? 37 :	    \
    (val) & (1ULL << 36) ? 36 :	    \
    (val) & (1ULL << 35) ? 35 :	    \
    (val) & (1ULL << 34) ? 34 :	    \
    (val) & (1ULL << 33) ? 33 :	    \
    (val) & (1ULL << 32) ? 32 :	    \
    (val) & (1ULL << 31) ? 31 :	    \
    (val) & (1ULL << 30) ? 30 :	    \
    (val) & (1ULL << 29) ? 29 :	    \
    (val) & (1ULL << 28) ? 28 :	    \
    (val) & (1ULL << 27) ? 27 :	    \
    (val) & (1ULL << 26) ? 26 :	    \
    (val) & (1ULL << 25) ? 25 :	    \
    (val) & (1ULL << 24) ? 24 :	    \
    (val) & (1ULL << 23) ? 23 :	    \
    (val) & (1ULL << 22) ? 22 :	    \
    (val) & (1ULL << 21) ? 21 :	    \
    (val) & (1ULL << 20) ? 20 :	    \
    (val) & (1ULL << 19) ? 19 :	    \
    (val) & (1ULL << 18) ? 18 :	    \
    (val) & (1ULL << 17) ? 17 :	    \
    (val) & (1ULL << 16) ? 16 :	    \
    (val) & (1ULL << 15) ? 15 :	    \
    (val) & (1ULL << 14) ? 14 :	    \
    (val) & (1ULL << 13) ? 13 :	    \
    (val) & (1ULL << 12) ? 12 :	    \
    (val) & (1ULL << 11) ? 11 :	    \
    (val) & (1ULL << 10) ? 10 :	    \
    (val) & (1ULL <<  9) ?  9 :	    \
    (val) & (1ULL <<  8) ?  8 :	    \
    (val) & (1ULL <<  7) ?  7 :	    \
    (val) & (1ULL <<  6) ?  6 :	    \
    (val) & (1ULL <<  5) ?  5 :	    \
    (val) & (1ULL <<  4) ?  4 :	    \
    (val) & (1ULL <<  3) ?  3 :	    \
    (val) & (1ULL <<  2) ?  2 :	    \
    1) : -1                         \
)

/**
 * ilog2_dynamic - log base 2 unsigned long value dynamically.
 * @val: dynamic long value to log base 2.
 */
static inline unsigned int ilog2_dynamic(unsigned long val)
{
    return fls(val) - 1;
}

/**
 * ilog2_64_dynamic - log base 2 64-bit unsigned value dynamically.
 * @val: dynamic 64-bit value to log base 2.
 */
static inline unsigned int ilog2_64_dynamic(unsigned long val)
{
    return fls64(val) - 1;
}

/**
 * roundup_pow2_dynamic - round up to nearest power of two dynamically.
 * @val: dynamic value to round up.
 */
static inline unsigned long roundup_pow2_dynamic(unsigned long val)
{
    return 1UL << fls(val - 1);
}

/**
 * rounddown_pow2_dynamic - round down to nearest power of two dynamically.
 * @val: dynamic value to round up.
 */
static inline unsigned long rounddown_pow2_dynamic(unsigned long val)
{
    return 1UL << (fls(val) - 1);
}

/**
 * is_power_of_2 - check if a value is a power of two.
 * @val: the value to check.
 */
static inline bool is_power_of_2(unsigned long val)
{
    return val != 0 && ((val & (val - 1)) == 0);
}

/**
 * ilog2 - log base 2 constant unsigned value.
 * @val: value to log2.
 */
#define ilog2(val) (                \
    __builtin_constant_p(val) ?     \
    ((val) < 2 ? 0 :                \
     63 - __builtin_clzll(val)) :   \
    (sizeof(val) <= 4) ?            \
    ilog2_dynamic(val) :            \
    ilog2_64_dynamic(val)           \
)

/**
 * roundup_power_of_2 - round up to nearest power of two.
 * @val: value to round up.
 */
#define roundup_power_of_2(val) (   \
    __builtin_constant_p(val) ?     \
    (((val) == 1) ? 1 : (1UL <<     \
    (ilog2((val) - 1) + 1))) :      \
    roundup_pow2_dynamic(val)       \
)

/**
 * rounddown_power_of_2 - round down to nearest power of two.
 * @val: value to round down.
 */
#define rounddown_power_of_2(val) ( \
    __builtin_constant_p(val) ?     \
    (1UL << ilog2(val)) :           \
    rounddown_pow2_dynamic(val)     \
)

#endif /* _LOG2_H_ */
