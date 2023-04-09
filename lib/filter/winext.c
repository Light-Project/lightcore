/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * lib/filter/winext.c: windowed extremum tracker.
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <filter/winext.h>
#include <kmalloc.h>
#include <export.h>

long winext_cmp_min(const struct winext_sample *sa,
                    const struct winext_sample *sb, const void *pdata)
{
    intptr_t valuea = (intptr_t)sa->value;
    intptr_t valueb = (intptr_t)sb->value;
    if (valuea == valueb) return 0;
    return valuea < valueb ? 1 : -1;
}
EXPORT_SYMBOL(winext_cmp_min);

long winext_cmp_max(const struct winext_sample *sa,
                    const struct winext_sample *sb, const void *pdata)
{
    intptr_t valuea = (intptr_t)sa->value;
    intptr_t valueb = (intptr_t)sb->value;
    if (valuea == valueb) return 0;
    return valuea > valueb ? 1 : -1;
}
EXPORT_SYMBOL(winext_cmp_max);

static void *winext_subwin(struct winext_state *winext, unsigned int win,
                           unsigned int time, void *value)
{
    struct winext_sample new = {.time = time, .value = value};
    unsigned int delta = time - winext->sample->time;
    unsigned int count;

    if (unlikely(delta > win)) do {
        for (count = 0; count < winext->num - 1; ++count)
            winext->sample[count] = winext->sample[count + 1];
        winext->sample[count] = new;
    } while (unlikely((delta = time - winext->sample->time) > win));

    else {
        for (count = 0; count < winext->num - 1; ++count) {
            if (unlikely(winext->sample[count].time == winext->sample[count + 1].time) &&
                (win >> (winext->num - count - 1)) < delta)
                break;
        }

        if (unlikely(count < winext->num - 1)) do
            winext->sample[++count] = new;
        while (count < winext->num);
    }

    return winext->sample->value;
}

void *winext_sample(struct winext_state *winext, winext_cmp_t cmp,
                    void *pdata, unsigned int win, unsigned int time, void *value)
{
    struct winext_sample new = {.time = time, .value = value};
    unsigned int count;

    if (unlikely(cmp(&new, winext->sample, pdata) >= 0) ||
        unlikely(time - winext->sample[winext->num - 1].time > win))
        return winext_reset(winext, time, value);

    for (count = 1; count < winext->num; ++count) {
        if (unlikely(cmp(&new, &winext->sample[count], pdata) >= 0))
            break;
    }

    while (count < winext->num)
        winext->sample[count++] = new;

    return winext_subwin(winext, win, time, value);
}
EXPORT_SYMBOL(winext_sample);

void *winext_reset(struct winext_state *winext, unsigned int time, void *value)
{
    struct winext_sample new = {.time = time, .value = value};
    unsigned int count;

    for (count = 0; count < winext->num; ++count)
        winext->sample[count] = new;

    return winext->sample[0].value;
}
EXPORT_SYMBOL(winext_reset);

void *winext_alloc(unsigned int num, gfp_t gfp)
{
    struct winext_state *winext;
    winext = kmalloc(sizeof(*winext) + (num * sizeof(*winext->sample)), gfp);
    winext->num = num;
    return winext;
}
EXPORT_SYMBOL(winext_alloc);

void winext_free(struct winext_state *winext)
{
    kfree(winext);
}
EXPORT_SYMBOL(winext_free);
