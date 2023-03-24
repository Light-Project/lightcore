/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HARDIRQ_H_
#define _HARDIRQ_H_

#include <preempt.h>

#define hardirq_start() ({              \
    preempt_count_add(HARDIRQ_OFFSET);  \
})

#define hardirq_end() ({                \
    preempt_count_sub(HARDIRQ_OFFSET);  \
})

extern void hardirq_entry(void);
extern void hardirq_exit(void);

#endif /* _HARDIRQ_H_ */
