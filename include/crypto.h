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

#define SHA1_DIGEST_SIZE        20
#define SHA1_BLOCK_SIZE         64
#define SHA1_DIGEST_WORDS       (SHA1_DIGEST_SIZE / BYTES_PER_U32)
#define SHA1_WORKSPACE_WORDS    (SHA1_BLOCK_SIZE / BYTES_PER_U32)

#define SHA1_DIGEST0 0x67452301UL
#define SHA1_DIGEST1 0xefcdab89UL
#define SHA1_DIGEST2 0x98badcfeUL
#define SHA1_DIGEST3 0x10325476UL
#define SHA1_DIGEST4 0xc3d2e1f0UL

extern void sha1_transform(uint32_t digest[SHA1_DIGEST_WORDS], uint32_t buff[SHA1_WORKSPACE_WORDS], const char *data);
extern void sha1_init(uint32_t digest[SHA1_DIGEST_WORDS]);

extern uint8_t crc4(uint8_t *src, int len, uint8_t crc);
extern uint8_t crc8(uint8_t *src, int len, uint8_t crc);
extern uint16_t crc16(uint8_t *src, int len, uint16_t crc);
extern uint32_t crc32(const uint8_t *s, int len, uint32_t crc);

#endif  /* _CRYPTO_H_ */
