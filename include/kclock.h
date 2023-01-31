/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCLOCK_H_
#define _KCLOCK_H_

#include <types.h>
#include <time.h>

struct kclock_data {
    uint64_t (*read)(void);
    uint64_t freq;

    uint64_t mask;
    uint64_t base;
    uint64_t cycle_last;

    unsigned int mult;
    unsigned int shift;
};

extern void kclock_register(uint64_t (*read)(void), uint64_t freq, uint64_t mask);
extern void kclock_update(void);

extern const struct kclock_data *kclock_read_start(unsigned int *seq);
extern bool kclock_read_retry(unsigned int seq);
extern uint64_t kclock_get(void);

#endif /* _KCLOCK_H_ */
