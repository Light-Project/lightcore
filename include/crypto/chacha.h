/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_CHACHA_H_
#define _CRYPTO_CHACHA_H_

#include <types.h>

#define CHACHA_IV_SIZE		16
#define CHACHA_KEY_SIZE		32
#define CHACHA_BLOCK_SIZE	64
#define CHACHAPOLY_IV_SIZE	12

extern void hchacha_block(uint32_t *state, uint8_t *stream, unsigned int rounds);
extern void chacha_block(uint32_t *state, uint8_t *stream, unsigned int rounds);
extern void chacha_init_const(uint32_t *state);
extern void chacha_init(uint32_t *state, const uint32_t *key, const uint8_t *iv);

static inline void chacha20_block(uint32_t *state, uint8_t *stream)
{
    chacha_block(state, stream, 20);
}

#endif  /* _CRYPTO_CHACHA_H_ */
