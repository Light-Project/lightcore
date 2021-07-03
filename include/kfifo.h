/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/include/kfifo.h
 *  light core kfifo lib
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-18      first version 
 * 
 */

#ifndef _KFIFO_H_
#define _KFIFO_H_
    
#include <system/spinlock.h>

struct kfifo{
    spinlock_t lock;
    char *buffer;
    size_t size;
    size_t in, out;
};

extern struct kfifo *kfifo_alloc(size_t size);
extern void kfifo_free(struct kfifo *fifo);
extern state kfifo_reset(struct kfifo *fifo);
extern size_t kfifo_len(struct kfifo *fifo);
extern state kfifo_read(struct kfifo *fifo, uint8_t *buff, size_t size);
extern state kfifo_write(struct kfifo *fifo, uint8_t *buff, size_t size);

#endif
