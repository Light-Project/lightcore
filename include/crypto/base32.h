/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_BASE32_H_
#define _CRYPTO_BASE32_H_

#include <types.h>
#include <state.h>
#include <math.h>

static inline size_t base32_encode_length(size_t size)
{
    return DIV_ROUND_UP(size, 5) * 8 + 1;
}

static inline size_t base32_decode_length(size_t size)
{
    return DIV_ROUND_UP(size, 8) * 5;
}

extern void base32_encode(void *buff, const void *data, size_t size);
extern state base32_decode(void *buff, const void *data, size_t size);

#endif  /* _CRYPTO_BASE32_H_ */
