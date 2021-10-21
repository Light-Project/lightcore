/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <string.h>
#include <kmalloc.h>
#include <kfifo.h>
#include <spinlock.h>
#include <export.h>

void kfifo_read(struct kfifo *fifo, uint8_t *buff, size_t size)
{
    size_t rsize; /* fold back length */

    if(!fifo || !buff)
        return;

    size = min(size, fifo->in - fifo->out);
    rsize = min(size, (fifo->size - fifo->out) & (fifo->size - 1));
    memcpy(buff, fifo->buffer + (fifo->out & (fifo->size - 1)), rsize);
    memcpy(buff + rsize, fifo->buffer, size - rsize);
    fifo->out += size;
}
EXPORT_SYMBOL(kfifo_read);

void kfifo_write(struct kfifo *fifo, uint8_t *buff, size_t size)
{
    size_t rsize; /* fold back length */

    if(!fifo || !buff)
        return;

    size = min(size, fifo->size - fifo->in + fifo->out);
    rsize = min(size, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buff, rsize);
	memcpy(fifo->buffer, buff + rsize, size - rsize);
    fifo->in += size;
}
EXPORT_SYMBOL(kfifo_write);

struct kfifo *kfifo_alloc(size_t size)
{
    struct kfifo *fifo;

    if (!size)
        return NULL;

    fifo = kmalloc(sizeof(struct kfifo) + size, GFP_KERNEL);
    if (!fifo)
        return NULL;

    fifo->buffer = (char *)fifo + sizeof(*fifo);
    fifo->size = size;
    fifo_reset(fifo);

    return fifo;
}
EXPORT_SYMBOL(kfifo_alloc);

void kfifo_free(struct kfifo *fifo)
{
    kfree(fifo);
}
EXPORT_SYMBOL(kfifo_free);
