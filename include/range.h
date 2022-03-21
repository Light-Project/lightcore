/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RANGE_H_
#define _RANGE_H_

/**
 * min - return minimum of two values of the same or compatible types
 * @a: first value
 * @b: second value
 */
#define min(a, b) ({                                    \
    typeof(a) _amin = (a);                              \
    typeof(a) _bmin = (b);                              \
    (void)(&_amin == &_bmin);                           \
    _amin < _bmin ? _amin : _bmin;                      \
})

/**
 * max - return maximum of two values of the same or compatible types
 * @a: first value
 * @b: second value
 */
#define max(a, b) ({                                    \
    typeof(a) _amax = (a);                              \
    typeof(a) _bmax = (b);                              \
    (void)(&_amax == &_bmax);                           \
    _amax > _bmax ? _amax : _bmax;                      \
})

/**
 * clamp - return a value clamped to a given range with strict typechecking.
 * @val: current value.
 * @lo: lowest allowable value.
 * @hi: highest allowable value.
 */
#define clamp(val, lo, hi) (                            \
    min(max(val, lo), hi)                               \
)

#endif  /* _RANGE_H_ */
