/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <win-minmax.h>
#include <export.h>

long minmax_cmp_min(const struct minmax_sample *sa,
                    const struct minmax_sample *sb, const void *pdata)
{
    uintptr_t valuea =  (uintptr_t)sa->value;
    uintptr_t valueb =  (uintptr_t)sb->value;
    if (valuea == valueb) return 0;
    return valuea > valueb ? 1 : -1;
}
EXPORT_SYMBOL(minmax_cmp_min);

long minmax_cmp_max(const struct minmax_sample *sa,
                    const struct minmax_sample *sb, const void *pdata)
{
    uintptr_t valuea =  (uintptr_t)sa->value;
    uintptr_t valueb =  (uintptr_t)sb->value;
    if (valuea == valueb) return 0;
    return valuea < valueb ? 1 : -1;
}
EXPORT_SYMBOL(minmax_cmp_max);

void *minmax_running(struct minmax_state *minmax, minmax_cmp_t cmp,
                     void *pdata, ktime_t win, ktime_t time, void *value)
{
    struct minmax_sample new = {.time = time, .value = value};
    unsigned int count;

    if (unlikely(cmp(&new, minmax->sample, pdata)) ||
        unlikely(time - minmax->sample[minmax->num - 1].time > win))
        return minmax_reset(minmax, time, value);

    for (count = 1; count < minmax->num; ++count) {
        if (unlikely(cmp(&new, &minmax->sample[count], pdata)))
            break;
    }

    while (unlikely(count < minmax->num))
        minmax->sample[count++] = new;

    while (unlikely(time - minmax->sample->time > win)) {
        for (count = 0; count < minmax->num - 1; ++count)
            minmax->sample[count] = minmax->sample[count + 1];
        minmax->sample[count] = new;
    }

    return minmax->sample->value;
}
EXPORT_SYMBOL(minmax_running);

void *minmax_reset(struct minmax_state *minmax, ktime_t time, void *value)
{
    struct minmax_sample new = {.time = time, .value = value};
    unsigned int count;

    for (count = 0; count < minmax->num; ++count)
        minmax->sample[count] = new;

    return minmax->sample[0].value;
}
EXPORT_SYMBOL(minmax_reset);
