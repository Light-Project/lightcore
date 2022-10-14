/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _WIN_MINMAX_H_
#define _WIN_MINMAX_H_

#include <types.h>
#include <stddef.h>

struct minmax_sample {
    ktime_t time;
    void *value;
};

struct minmax_state {
    unsigned int num;
    struct minmax_sample sample[0];
};

typedef long (*minmax_cmp_t)(const struct minmax_sample *sa, const struct minmax_sample *sb, const void *pdata);
extern long minmax_cmp_mix(const struct minmax_sample *sa, const struct minmax_sample *sb, const void *pdata);
extern long minmax_cmp_max(const struct minmax_sample *sa, const struct minmax_sample *sb, const void *pdata);
extern void *minmax_running(struct minmax_state *minmax, minmax_cmp_t cmp, void *pdata, ktime_t win, ktime_t time, void *value);
extern void *minmax_reset(struct minmax_state *minmax, ktime_t time, void *value);

#endif /* _WIN_MINMAX_H_ */
