/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TICKTIME_H_
#define _TICKTIME_H_

#include <types.h>

extern volatile ttime_t ticktime;

#define SYSTICK_NSEC        \
    DIV_ROUND_CLOSEST(      \
        NSEC_PER_SEC,       \
        CONFIG_SYSTICK_FREQ \
)

/**
 * ttime_compare - compares two ttime_t variables
 * @timea: variables a
 * @timeb: variables b
 */
static inline int ttime_compare(const ttime_t timea, const ttime_t timeb)
{
    if (timea < timeb)
        return -1;
    if (timea > timeb)
        return 1;
    return 0;
}

/**
 * ttime_after - compare if a ttime value is bigger than another one
 * @timea: the bigger one
 * @timeb: the smaller one
 */
static inline bool ttime_after(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) > 0;
}

/**
 * ttime_before - compare if a ttime value is smaller than another one
 * @timea: the smaller one
 * @timeb: the bigger one
 */
static inline bool ttime_before(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) < 0;
}

/**
 * ttime_after - compare if a ttime value is bigger or equal than another one
 * @timea: the bigger one
 * @timeb: the smaller one
 */
static inline bool ttime_after_equal(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) >= 0;
}

/**
 * ttime_before_equal - compare if a ttime value is smaller than or equal another one
 * @timea: the smaller one
 * @timeb: the bigger one
 */
static inline bool ttime_before_equal(const ttime_t timea, const ttime_t timeb)
{
    return ttime_compare(timea, timeb) <= 0;
}

#endif  /* _TICKTIME_H_ */
