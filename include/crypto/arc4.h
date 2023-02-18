/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_ARC4_H_
#define _CRYPTO_ARC4_H_

#include <types.h>

#define ARC4_KEY_SIZE   256

struct arc4_context {
    uint8_t box[ARC4_KEY_SIZE];
    uint8_t proa;
    uint8_t prob;
};

extern void arc4_transform(struct arc4_context *ctx, void *buff, const void *data, size_t size);
extern void arc4_setkey(struct arc4_context *ctx, const uint8_t *key, unsigned int klen);

#endif /* _CRYPTO_ARC4_H_ */
