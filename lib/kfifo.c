/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <kfifo.h>
#include <log2.h>
#include <string.h>
#include <export.h>

#define KFIFO_GENERIC_COPY(copy1, copy2, fold1, fold2) do { \
    unsigned long size = kfifo->mask + 1;                   \
    unsigned long esize = kfifo->esize;                     \
    unsigned long llen;                                     \
                                                            \
    offset &= kfifo->mask;                                  \
    if (esize != 1) {                                       \
        offset *= esize;                                    \
        size *= esize;                                      \
        len *= esize;                                       \
    }                                                       \
                                                            \
    llen = min(len, size - offset);                         \
    memcpy(copy1, copy2, llen);                             \
    memcpy(fold1, fold2, len - llen);                       \
    smp_wmb();                                              \
} while (0)

static __always_inline void
kfifo_out_copy(struct kfifo *kfifo, void *buff, unsigned long len, unsigned long offset)
{
    KFIFO_GENERIC_COPY(
        buff, kfifo->data + offset,
        buff + llen, kfifo->data
    );
}

static __always_inline void
kfifo_in_copy(struct kfifo *kfifo, const void *buff, unsigned long len, unsigned long offset)
{
    KFIFO_GENERIC_COPY(
        kfifo->data + offset, buff,
        kfifo->data, buff + llen
    );
}

static __always_inline unsigned long
kfifo_record_peek(struct kfifo *kfifo, unsigned long recsize)
{
    unsigned long mask = kfifo->mask;
    unsigned long offset = kfifo->out;
    unsigned long length = 0;
    uint8_t *data = kfifo->data;

    if (kfifo->esize != 1) {
        offset *= kfifo->esize;
        mask *= kfifo->esize;
        mask += kfifo->esize - 1;
    }

    while (recsize--) {
        length <<= BITS_PER_U8;
        length |= data[offset & mask];
        offset += kfifo->esize;
    }

    return length;
}

static __always_inline void
kfifo_record_poke(struct kfifo *kfifo, unsigned long len, unsigned long recsize)
{
    unsigned long mask = kfifo->mask;
    unsigned long offset = kfifo->out;
    uint8_t *data = kfifo->data;

    if (kfifo->esize != 1) {
        offset *= kfifo->esize;
        mask *= kfifo->esize;
        mask += kfifo->esize - 1;
    }

    while (recsize--) {
        data[offset & mask] = (uint8_t)len;
        offset += kfifo->esize;
        len >>= BITS_PER_U8;
    }
}

static inline bool kfifo_empty(struct kfifo *kfifo)
{
    return kfifo->in == kfifo->out;
}

static inline unsigned long kfifo_valid(struct kfifo *kfifo)
{
    return kfifo->in - kfifo->out;
}

static inline unsigned long kfifo_unused(struct kfifo *kfifo)
{
    return (kfifo->mask + 1) - (kfifo->in - kfifo->out);
}

unsigned long kfifo_peek_flat(struct kfifo *kfifo, void *buff, unsigned long len)
{
    unsigned long valid;

    valid = kfifo_valid(kfifo);
    min_adj(len, valid);
    kfifo_out_copy(kfifo, buff, len, kfifo->out);

    return len;
}
EXPORT_SYMBOL(kfifo_peek_flat);

unsigned long kfifo_out_flat(struct kfifo *kfifo, void *buff, unsigned long len)
{
    unsigned long llen;

    llen = kfifo_peek_flat(kfifo, buff, len);
    kfifo->out += llen;

    return llen;
}
EXPORT_SYMBOL(kfifo_out_flat);

unsigned long kfifo_in_flat(struct kfifo *kfifo, const void *buff, unsigned long len)
{
    unsigned long unused;

    unused = kfifo_unused(kfifo);
    min_adj(len, unused);
    kfifo_in_copy(kfifo, buff, len, kfifo->in);
    kfifo->in += len;

    return len;
}
EXPORT_SYMBOL(kfifo_in_flat);

unsigned long kfifo_peek_record(struct kfifo *kfifo, void *buff,
                                unsigned long len, unsigned long record)
{
    unsigned long datalen;

    if (kfifo_empty(kfifo))
        return 0;

    datalen = kfifo_record_peek(kfifo, record);
    min_adj(len, datalen);
    kfifo_out_copy(kfifo, buff, len, kfifo->out + record);

    return len;
}
EXPORT_SYMBOL(kfifo_peek_record);

unsigned long kfifo_out_record(struct kfifo *kfifo, void *buff,
                               unsigned long len, unsigned long record)
{
    unsigned long datalen;

    if (kfifo_empty(kfifo))
        return 0;

    datalen = kfifo_record_peek(kfifo, record);
    min_adj(len, datalen);
    kfifo_out_copy(kfifo, buff, len, kfifo->out + record);
	kfifo->out += datalen + record;

    return len;
}
EXPORT_SYMBOL(kfifo_out_record);

unsigned long kfifo_in_record(struct kfifo *kfifo, const void *buff,
                              unsigned long len, unsigned long record)
{
    if (len + record > kfifo_unused(kfifo))
        return 0;

    kfifo_record_poke(kfifo, len, record);
    kfifo_in_copy(kfifo, buff, len, kfifo->in + record);
    kfifo->in += len + record;

    return len;
}
EXPORT_SYMBOL(kfifo_in_record);

state kfifo_dynamic_alloc(struct kfifo *kfifo, size_t esize, size_t size, gfp_t flags)
{
    size = roundup_power_of_2(size);
    if (size < 2)
        return -EINVAL;

    kfifo->in = 0;
    kfifo->out = 0;
    kfifo->mask = size - 1;
    kfifo->esize = esize;

    kfifo->data = kmalloc_array(size, esize, flags);
    if (!kfifo->data) {
        kfifo->mask = 0;
        return -ENOMEM;
    }

    return -ENOERR;
}
EXPORT_SYMBOL(kfifo_dynamic_alloc);

void kfifo_dynamic_free(struct kfifo *kfifo)
{
    kfree(kfifo->data);
    kfifo->in = 0;
    kfifo->out = 0;
    kfifo->mask = 0;
    kfifo->esize = 0;
    kfifo->data = NULL;
}
EXPORT_SYMBOL(kfifo_dynamic_free);
