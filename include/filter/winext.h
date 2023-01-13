/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_WINEXT_H_
#define _FILTER_WINEXT_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>
#include <mm/gfp.h>

struct winext_sample {
    unsigned int time;
    void *value;
};

struct winext_state {
    unsigned int num;
    struct winext_sample sample[0];
};

typedef long (*winext_cmp_t)(const struct winext_sample *sa, const struct winext_sample *sb, const void *pdata);
extern long winext_cmp_min(const struct winext_sample *sa, const struct winext_sample *sb, const void *pdata);
extern long winext_cmp_max(const struct winext_sample *sa, const struct winext_sample *sb, const void *pdata);

extern void *winext_sample(struct winext_state *winext, winext_cmp_t cmp, void *pdata, unsigned int win, unsigned int time, void *value);
extern void *winext_reset(struct winext_state *winext, unsigned int time, void *value);
extern void *winext_alloc(unsigned int num, gfp_t gfp);
extern void winext_free(struct winext_state *winext);

static inline intptr_t winext_sample_min(struct winext_state *winext, unsigned int win, unsigned int time, intptr_t value)
{
    return (intptr_t)winext_sample(winext, winext_cmp_min, NULL, win, time, (void *)value);
}

static inline intptr_t winext_sample_max(struct winext_state *winext, unsigned int win, unsigned int time, intptr_t value)
{
    return (intptr_t)winext_sample(winext, winext_cmp_max, NULL, win, time, (void *)value);
}

static inline intptr_t winext_init_min(struct winext_state *winext)
{
    return (intptr_t)winext_reset(winext, 0, (void *)LONG_MAX);
}

static inline intptr_t winext_init_max(struct winext_state *winext)
{
    return (intptr_t)winext_reset(winext, 0, (void *)LONG_MIN);
}

#endif /* _FILTER_WINEXT_H_ */
