/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/lib/kfifo.c
 *  light core kfifo lib
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-18      first version 
 * 
 */

#include <mm.h>
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <state.h>
#include <system/spinlock.h>
#include <export.h>
#include <kfifo.h>

static inline struct kfifo *fifo_alloc(size_t size)
{
    struct kfifo *fifo = NULL;

    fifo = kmalloc(sizeof(struct kfifo), GFP_KERNEL);
    if(fifo == NULL)
        return NULL;
    fifo->buffer = kmalloc(size, GFP_KERNEL);
    if(fifo->buffer == NULL)
    {
        kfree(fifo);
        return NULL;
    }
    fifo->size = size;
    fifo->in   = 0;
    fifo->out  = 0;
    return fifo;
}

static inline void fifo_free(struct kfifo *fifo)
{
    kfree(fifo->buffer);
    kfree(fifo);
}

static inline void fifo_reset(struct kfifo *fifo)
{
    fifo->in   = 0;
    fifo->out  = 0;
}

static inline size_t fifo_len(struct kfifo *fifo)
{
    return fifo->in - fifo->out;
}

static inline state fifo_read(struct kfifo *fifo, uint8_t *buff, size_t size)
{
    size_t rsize = 0;
    
    size = min(size, fifo->in - fifo->out);
    rsize = min(size, (fifo->size - fifo->out) & (fifo->size - 1));
    memcpy(buff, fifo->buffer + (fifo->out & (fifo->size - 1)), rsize);
    memcpy(buff + rsize, fifo->buffer, size - rsize);
    fifo->out += size;

    return -ENOERR;
}

static inline state fifo_write(struct kfifo *fifo, uint8_t *buff, size_t size)
{   
    size_t rsize = 0;

    size = min(size, fifo->size - fifo->in + fifo->out);
    // smp_mb();
    rsize = min(size, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buff, rsize);
	memcpy(fifo->buffer, buff + rsize, size - rsize);
    // smp_mb();
    fifo->in += size;

    return -ENOERR;
}

/**
 * External function
 */

struct kfifo *kfifo_alloc(size_t size)
{
    if(size == 0)
        return NULL;
    return fifo_alloc(size);
}
EXPORT_SYMBOL(kfifo_alloc);

void kfifo_free(struct kfifo *fifo)
{
    if(fifo == NULL)
        return;
    fifo_free(fifo);
}
EXPORT_SYMBOL(kfifo_free);

state kfifo_reset(struct kfifo *fifo)
{
    if(fifo == NULL)
        return -EINVAL;
    fifo_reset(fifo);
    return -ENOERR;
}
EXPORT_SYMBOL(kfifo_reset);

size_t kfifo_len(struct kfifo *fifo)
{
    if(fifo == NULL)
        return 0;
    return fifo_len(fifo);
}
EXPORT_SYMBOL(kfifo_len);

state kfifo_read(struct kfifo *fifo, uint8_t *buff, size_t size)
{
    if((fifo==NULL)||(buff==NULL)||(size>fifo->size))
        return -EINVAL;
    spin_lock(&fifo->lock);
    fifo_read(fifo, buff, size);
    if(fifo->in == fifo->out)
        fifo_reset(fifo);
    spin_unlock(&fifo->lock);
    return -ENOERR;
}
EXPORT_SYMBOL(kfifo_read);

state kfifo_write(struct kfifo *fifo, uint8_t *buff, size_t size)
{
    if((fifo==NULL)||(buff==NULL)||(size>fifo->size))
        return -EINVAL;
    spin_lock(&fifo->lock);
    fifo_write(fifo, buff, size);
    spin_unlock(&fifo->lock);
    return -ENOERR;
}
EXPORT_SYMBOL(kfifo_write);

