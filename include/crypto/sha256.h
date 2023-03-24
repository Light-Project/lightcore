/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_SHA256_H_
#define _CRYPTO_SHA256_H_

#include <types.h>

#define SHA224_DIGEST0 0xc1059ed8UL
#define SHA224_DIGEST1 0x367cd507UL
#define SHA224_DIGEST2 0x3070dd17UL
#define SHA224_DIGEST3 0xf70e5939UL
#define SHA224_DIGEST4 0xffc00b31UL
#define SHA224_DIGEST5 0x68581511UL
#define SHA224_DIGEST6 0x64f98fa7UL
#define SHA224_DIGEST7 0xbefa4fa4UL

#define SHA256_DIGEST0 0x6a09e667UL
#define SHA256_DIGEST1 0xbb67ae85UL
#define SHA256_DIGEST2 0x3c6ef372UL
#define SHA256_DIGEST3 0xa54ff53aUL
#define SHA256_DIGEST4 0x510e527fUL
#define SHA256_DIGEST5 0x9b05688cUL
#define SHA256_DIGEST6 0x1f83d9abUL
#define SHA256_DIGEST7 0x5be0cd19UL

extern void sha256_transform(uint32_t *digest, uint32_t *buff, const char *data);
extern void sha224_transform(uint32_t *digest, uint32_t *buff, const char *data);
extern void sha256_init(uint32_t *digest);
extern void sha224_init(uint32_t *digest);

#endif /* _CRYPTO_SHA256_H_ */
