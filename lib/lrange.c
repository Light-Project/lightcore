/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <lrange.h>
#include <math.h>
#include <export.h>

unsigned int lrange_value_limit(const struct lrange *lrange)
{
    return lrange->base + (lrange->max - lrange->min) * lrange->step;
}
EXPORT_SYMBOL(lrange_value_limit);

unsigned int lrange_selector_range(const struct lrange *lrange)
{
    return lrange->max - lrange->min + 1;
}
EXPORT_SYMBOL(lrange_selector_range);

#define GENERIC_LRANGE_GET_VALUE_OP(name, expression)                                           \
state lrange_get_value_##name                                                                   \
(const struct lrange *lrange, unsigned int selector, unsigned int *value)                       \
{                                                                                               \
    if (selector < lrange->min || selector > lrange->max)                                       \
        return -EINVAL;                                                                         \
                                                                                                \
    *value = expression;                                                                        \
    return -ENOERR;                                                                             \
}                                                                                               \
EXPORT_SYMBOL(lrange_get_value_##name)

GENERIC_LRANGE_GET_VALUE_OP(low, lrange->base + (selector - lrange->min) * lrange->step);
GENERIC_LRANGE_GET_VALUE_OP(high, lrange->base + (selector - lrange->min) * lrange->step + (lrange->step - 1));

#define GENERIC_LRANGE_GET_VALUE_WITHIN_OP(name, expression)                                    \
state lrange_get_value_##name##_within                                                          \
(const struct lrange *lrange, unsigned int selector, unsigned int *value)                       \
{                                                                                               \
    if (selector < lrange->min) {                                                               \
        *value = lrange->base;                                                                  \
        return -EINVAL;                                                                         \
    }                                                                                           \
                                                                                                \
    if (selector > lrange->max) {                                                               \
        *value = lrange_value_limit(lrange);                                                    \
        return -EINVAL;                                                                         \
    }                                                                                           \
                                                                                                \
    *value = expression;                                                                        \
    return -ENOERR;                                                                             \
}                                                                                               \
EXPORT_SYMBOL(lrange_get_value_##name##_within)

GENERIC_LRANGE_GET_VALUE_WITHIN_OP(low, lrange->base + (selector - lrange->min) * lrange->step);
GENERIC_LRANGE_GET_VALUE_WITHIN_OP(high, lrange->base + (selector - lrange->min) * lrange->step + (lrange->step - 1));

#define GENERIC_LRANGE_GET_SELECTOR_OP(name, hstep, nstep)                                      \
state lrange_get_selector_##name                                                                \
(const struct lrange *lrange, unsigned int value, unsigned int *selector)                       \
{                                                                                               \
    if (value < lrange->base || value > lrange_value_limit(lrange))                             \
        return -EINVAL;                                                                         \
                                                                                                \
    if (lrange->step)                                                                           \
        *selector = hstep;                                                                      \
    else                                                                                        \
        *selector = nstep;                                                                      \
                                                                                                \
    return -ENOERR;                                                                             \
}                                                                                               \
EXPORT_SYMBOL(lrange_get_selector_##name)

GENERIC_LRANGE_GET_SELECTOR_OP(low, lrange->min + (value - lrange->base) / lrange->step, lrange->min);
GENERIC_LRANGE_GET_SELECTOR_OP(high, lrange->min + DIV_ROUND_UP(value - lrange->base, lrange->step), lrange->max);

#define GENERIC_LRANGE_GET_SELECTOR_WITHIN_OP(name, hstep, nstep)                               \
state lrange_get_selector_##name##_within                                                       \
(const struct lrange *lrange, unsigned int value, unsigned int *selector)                       \
{                                                                                               \
    if (value < lrange->base) {                                                                 \
        *selector = lrange->min;                                                                \
        return -EINVAL;                                                                         \
    }                                                                                           \
                                                                                                \
    if (value > lrange_value_limit(lrange)) {                                                   \
        *selector = lrange->max;                                                                \
        return -EINVAL;                                                                         \
    }                                                                                           \
                                                                                                \
    if (lrange->step)                                                                           \
        *selector = hstep;                                                                      \
    else                                                                                        \
        *selector = nstep;                                                                      \
                                                                                                \
    return -ENOERR;                                                                             \
}                                                                                               \
EXPORT_SYMBOL(lrange_get_selector_##name##_within)

GENERIC_LRANGE_GET_SELECTOR_WITHIN_OP(low, lrange->min + (value - lrange->base) / lrange->step, lrange->min);
GENERIC_LRANGE_GET_SELECTOR_WITHIN_OP(high, lrange->min + DIV_ROUND_UP(value - lrange->base, lrange->step), lrange->max);

#define GENERIC_LRANGE_ARRAY_GET_OP(name)                                                       \
state lrange_array_get##name                                                                    \
(const struct lrange *lrange, unsigned int num, unsigned int in, unsigned int *out)             \
{                                                                                               \
    unsigned int count;                                                                         \
    state retval = -EINVAL;                                                                     \
                                                                                                \
    for (count = 0; count < num; ++count) {                                                     \
        retval = lrange_get_##name(lrange + num, in, out);                                      \
        if (!retval)                                                                            \
            break;                                                                              \
    }                                                                                           \
                                                                                                \
    return retval;                                                                              \
}                                                                                               \
EXPORT_SYMBOL(lrange_array_get##name)

GENERIC_LRANGE_ARRAY_GET_OP(value_low);
GENERIC_LRANGE_ARRAY_GET_OP(value_high);
GENERIC_LRANGE_ARRAY_GET_OP(value_low_within);
GENERIC_LRANGE_ARRAY_GET_OP(value_high_within);
GENERIC_LRANGE_ARRAY_GET_OP(selector_low);
GENERIC_LRANGE_ARRAY_GET_OP(selector_high);
GENERIC_LRANGE_ARRAY_GET_OP(selector_low_within);
GENERIC_LRANGE_ARRAY_GET_OP(selector_high_within);
