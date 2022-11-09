/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MINMAX_H_
#define _MINMAX_H_

/**
 * min - return minimum of two values of the same or compatible types.
 * @a: first value.
 * @b: second value.
 */
#define min(a, b) ({                \
    typeof(a) _amin = (a);          \
    typeof(a) _bmin = (b);          \
    (void)(&_amin == &_bmin);       \
    _amin < _bmin ? _amin : _bmin;  \
})

/**
 * max - return maximum of two values of the same or compatible types.
 * @a: first value.
 * @b: second value.
 */
#define max(a, b) ({                \
    typeof(a) _amax = (a);          \
    typeof(a) _bmax = (b);          \
    (void)(&_amax == &_bmax);       \
    _amax > _bmax ? _amax : _bmax;  \
})

/**
 * clamp - return a value clamped to a given range with strict typechecking.
 * @val: current value.
 * @lo: lowest allowable value.
 * @hi: highest allowable value.
 */
#define clamp(val, lo, hi) (        \
    min(max(val, lo), hi)           \
)

/**
 * min_adj - Adjust the minimum value of @val.
 * @val: value to adjust.
 * @lo: lowest allowable value.
 */
#define min_adj(val, lo) ({         \
    (val) = min(val, lo);           \
})

/**
 * max_adj - Adjust the maximum value of @val.
 * @val: value to adjust.
 * @hi: highest allowable value.
 */
#define max_adj(val, hi) ({         \
    (val) = max(val, hi);           \
})

/**
 * clamp_adj - Adjust the clamped value of @val.
 * @val: value to adjust.
 * @lo: lowest allowable value.
 * @hi: highest allowable value.
 */
#define clamp_adj(val, lo, hi) ({   \
    (val) = clamp(val, lo, hi);     \
})

#endif  /* _MINMAX_H_ */
