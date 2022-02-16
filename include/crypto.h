/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <kernel.h>

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

extern uint8_t crc4(uint8_t *src, int len, uint8_t crc);
extern uint8_t crc8(uint8_t *src, int len, uint8_t crc);
extern uint16_t crc16(uint8_t *src, int len, uint16_t crc);
extern uint32_t crc32(const uint8_t *s, int len, uint32_t crc);

#endif  /* _CRYPTO_H_ */
