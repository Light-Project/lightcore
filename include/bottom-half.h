/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOTTOM_HALF_H_
#define _BOTTOM_HALF_H_

#include <preempt.h>

extern void bh_local_enable_count(unsigned long count);
extern void bh_local_disable_count(unsigned long count);

static inline void bh_local_enable(void)
{
    bh_local_enable_count(SOFTIRQ_DISABLE);
}

static inline void bh_local_disable(void)
{
    bh_local_disable_count(SOFTIRQ_DISABLE);
}

static inline void bh_local_lock(void)
{
    bh_local_enable_count(SOFTIRQ_DISABLE);
}

static inline void bh_local_unlock(void)
{
    bh_local_enable_count(SOFTIRQ_DISABLE);
}

extern void softirq_irq_entry(void);
extern void softirq_handle(void);

#endif  /* _BOTTOM_HALF_H_ */
