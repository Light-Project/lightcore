/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_ASCII85_H_
#define _CRYPTO_ASCII85_H_

#include <types.h>
#include <state.h>

extern void ascii85_encode(void *buff, const void *data, size_t size);
extern state ascii85_decode(void *buff, const void *data, size_t size);
extern size_t ascii85_encode_length(const void *data, size_t size);
extern size_t ascii85_decode_length(const void *data, size_t size);

#endif  /* _CRYPTO_ASCII85_H_ */
