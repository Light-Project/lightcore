/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPLETION_H_
#define _COMPLETION_H_

#include <waitqueue.h>

struct completion {
    struct wait_queue_head queue;
    unsigned int counter;
};

#define COMPLETION_STATIC(name) {               \
    .queue = WAITQUEUE_STATIC((name).queue),    \
    .counter = 0,                               \
}

#define COMPLETION_INIT(name) \
    (struct completion) COMPLETION_STATIC(name)

#define DEFINE_COMPLETION(name) \
    struct completion name = COMPLETION_INIT(name)

static inline void completion_init(struct completion *comp)
{
    *comp = COMPLETION_INIT(*comp);
}

extern void completion_wait(struct completion *comp);
extern ttime_t completion_wait_timeout(struct completion *comp, ttime_t timeout);
extern void complete_one(struct completion *comp);
extern void complete_all(struct completion *comp);
extern bool completion_done(struct completion *comp);

#endif  /* _COMPLETION_H_ */
