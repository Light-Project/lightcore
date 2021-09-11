#ifndef _SEQLOCK_H_
#define _SEQLOCK_H_

typedef struct {
    unsigned int sequence;
    spinlock_t lock;
} seqlock_t;

#endif
