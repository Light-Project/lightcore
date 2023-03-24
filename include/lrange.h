/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LRANGE_H_
#define _LRANGE_H_

#include <types.h>
#include <state.h>

/**
 * struct lrange - linear interval mapping.
 * @base: base value in linear range.
 * @min: lowest selector for linear range.
 * @max: highest selector for linear range.
 * @step: value step size.
 */
struct lrange {
    unsigned int base;
    unsigned int min, max;
    unsigned int step;
};

#define LRANGE_STATIC(BASE, MIN, MAX, STEP) \
    {.base = BASE, .min = MIN, .max = MAX, .step = STEP}

#define LRANGE_INIT(BASE, MIN, MAX, STEP) \
    (struct lrange) LRANGE_STATIC(BASE, MIN, MAX, STEP)

#define DEFINE_LRANGE(NAME, BASE, MIN, MAX, STEP) \
    struct lrange NAME = LRANGE_INIT(BASE, MIN, MAX, STEP)

extern unsigned int lrange_value_limit(const struct lrange *lrange);
extern unsigned int lrange_selector_range(const struct lrange *lrange);

extern state lrange_get_value_low(const struct lrange *lrange, unsigned int selector, unsigned int *value);
extern state lrange_get_value_high(const struct lrange *lrange, unsigned int selector, unsigned int *value);
extern state lrange_get_value_low_within(const struct lrange *lrange, unsigned int selector, unsigned int *value);
extern state lrange_get_value_high_within(const struct lrange *lrange, unsigned int selector, unsigned int *value);
extern state lrange_get_selector_low(const struct lrange *lrange, unsigned int value, unsigned int *selector);
extern state lrange_get_selector_high(const struct lrange *lrange, unsigned int value, unsigned int *selector);
extern state lrange_get_selector_low_within(const struct lrange *lrange, unsigned int value, unsigned int *selector);
extern state lrange_get_selector_high_within(const struct lrange *lrange, unsigned int value, unsigned int *selector);

extern state lrange_array_get_value_low(const struct lrange *lrange, unsigned int num, unsigned int selector, unsigned int *value);
extern state lrange_array_get_value_high(const struct lrange *lrange, unsigned int num, unsigned int selector, unsigned int *value);
extern state lrange_array_get_value_low_within(const struct lrange *lrange, unsigned int num, unsigned int selector, unsigned int *value);
extern state lrange_array_get_value_high_within(const struct lrange *lrange, unsigned int num, unsigned int selector, unsigned int *value);
extern state lrange_array_get_selector_low(const struct lrange *lrange, unsigned int num, unsigned int value, unsigned int *selector);
extern state lrange_array_get_selector_high(const struct lrange *lrange, unsigned int num, unsigned int value, unsigned int *selector);
extern state lrange_array_get_selector_low_within(const struct lrange *lrange, unsigned int num, unsigned int value, unsigned int *selector);
extern state lrange_array_get_selector_high_within(const struct lrange *lrange, unsigned int num, unsigned int value, unsigned int *selector);

#endif /* _LRANGE_H_ */
