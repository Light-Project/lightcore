/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_SHA1_H_
#define _CRYPTO_SHA1_H_

#include <types.h>

#define SHA1_DIGEST0 0x67452301UL
#define SHA1_DIGEST1 0xefcdab89UL
#define SHA1_DIGEST2 0x98badcfeUL
#define SHA1_DIGEST3 0x10325476UL
#define SHA1_DIGEST4 0xc3d2e1f0UL

#define SHA1_DIGEST_SIZE        20
#define SHA1_WORKSPACE_WORDS    16
#define SHA1_BLOCK_SIZE         64
#define SHA1_DIGEST_WORDS       (SHA1_DIGEST_SIZE / BYTES_PER_U32)

extern void sha1_transform(uint32_t *digest, uint32_t *buff, const char *data);
extern void sha1_init(uint32_t *digest);

#endif  /* _CRYPTO_SHA1_H_ */
