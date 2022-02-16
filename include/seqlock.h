/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SEQLOCK_H_
#define _SEQLOCK_H_

#include <spinlock.h>

typedef struct seqlock {
    unsigned int sequence;
    spinlock_t lock;
} seqlock_t;

#define SEQ_LOCK(name) \
    struct seqlock name;

/**
 * seqlock_read_start -
 *
 */
static inline unsigned int seqlock_read_start(struct seqlock *seq)
{
    unsigned int start;

    while ((start = READ_ONCE(seq->sequence)) & 1)
        cpu_relax();

    return start;
}

/**
 * seqlock_read_retry -
 *
 */
static inline bool seqlock_read_retry(struct seqlock *seq, unsigned int start)
{
    smp_rmb();
    return unlikely(READ_ONCE(seq->sequence) != start);
}

/**
 * seqlock_write_start -
 *
 */
static inline void seqlock_write_start(struct seqlock *seq)
{


}

/**
 * seqlock_write_end -
 *
 */
static inline void seqlock_write_end(struct seqlock *seq)
{

}

#endif
