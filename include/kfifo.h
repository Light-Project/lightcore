/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KFIFO_H_
#define _KFIFO_H_

#include <types.h>

struct kfifo{
    char *buffer;
    size_t in, out;
    size_t size;
};

static inline size_t fifo_len(struct kfifo *fifo)
{
    return fifo->in - fifo->out;
}

static inline void fifo_reset(struct kfifo *fifo)
{
    fifo->in = fifo->out = 0;
}

void kfifo_read(struct kfifo *fifo, uint8_t *buff, size_t size);
void kfifo_write(struct kfifo *fifo, uint8_t *buff, size_t size);
struct kfifo *kfifo_alloc(size_t size);
void kfifo_free(struct kfifo *fifo);

#endif
