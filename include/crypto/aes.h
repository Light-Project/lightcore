/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_AES_H_
#define _CRYPTO_AES_H_

#include <types.h>
#include <stddef.h>

#define AES_KEYSIZE_128     16
#define AES_KEYSIZE_192     24
#define AES_KEYSIZE_256     32

#define AES_MIN_KEY_SIZE    16
#define AES_MAX_KEY_SIZE    32
#define AES_BLOCK_SIZE      16
#define AES_MAX_KEYSIZE     (15 * 16)
#define AES_MAX_KEYWORDS    (AES_MAX_KEYSIZE / BYTES_PER_U32)

struct aes_context {
    uint32_t encrypt_key[AES_MAX_KEYWORDS];
    uint32_t decrypt_key[AES_MAX_KEYWORDS];
    unsigned int keylen;
};

static inline bool aes_check_keylen(unsigned int keylen)
{
    switch (keylen) {
        case AES_KEYSIZE_128:
        case AES_KEYSIZE_192:
        case AES_KEYSIZE_256:
            return true;

        default:
            return false;
    }
}

extern void aes_encrypt(struct aes_context *ctx, const uint8_t *src, uint8_t *dest);
extern void aes_decrypt(struct aes_context *ctx, const uint8_t *src, uint8_t *dest);
extern state aes_expandkey(struct aes_context *ctx, const uint8_t *inkey, unsigned int keylen);

#endif  /* _CRYPTO_AES_H_ */
