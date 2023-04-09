/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/sha1.h>
#include <bitops.h>
#include <proc.h>
#include <export.h>

#define W(count, buff) (buff[(count) & 0x0f])
#define SHA1_MIX(c, b, d) rol32(W(c + 13, b) ^ W(c + 8, b) ^ W(c +  2, b) ^ W(c, b), 1)
#define SHA1_SRC(c, b, d) unaligned_get_be32((uint32_t *)d + c)

#define SHA1_ROUND(count, buff, data, A, B, C, D, E, func, const, exp) do { \
    uint32_t value = func(count, buff, data);                               \
    W(count, buff) = value; E += value + rol32(A, 5) + (exp) + (const);     \
    B = ror32(B, 2); value = E; E = D; D = C; C = B; B = A; A = value;      \
} while (0)

#define SHA1_00_15(c, b, d, A, B, C, D, E) \
        SHA1_ROUND(c, b, d, A, B, C, D, E, SHA1_MIX, 0x5a827999, (((C ^ D) & B) ^ D))
#define SHA1_16_19(c, b, d, A, B, C, D, E) \
        SHA1_ROUND(c, b, d, A, B, C, D, E, SHA1_SRC, 0x5a827999, (((C ^ D) & B) ^ D))
#define SHA1_20_39(c, b, d, A, B, C, D, E) \
        SHA1_ROUND(c, b, d, A, B, C, D, E, SHA1_SRC, 0x6ed9eba1, (B ^ C ^ D))
#define SHA1_40_59(c, b, d, A, B, C, D, E) \
        SHA1_ROUND(c, b, d, A, B, C, D, E, SHA1_SRC, 0x8f1bbcdc, ((B & C) + (D & (B ^ C))))
#define SHA1_60_79(c, b, d, A, B, C, D, E) \
        SHA1_ROUND(c, b, d, A, B, C, D, E, SHA1_SRC, 0xca62c1d6, (B ^ C ^ D))

void sha1_transform(uint32_t *digest, uint32_t *buff, const char *data)
{
    uint32_t A, B, C, D, E;
    unsigned int count;

    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];

	/* Round 1 MIX */
    for (count = 0; count < 16; ++count)
        SHA1_00_15(count, buff, data, A, B, C, D, E);

	/* Round 1 SRC */
    for (count = 16; count < 20; ++count)
        SHA1_16_19(count, buff, data, A, B, C, D, E);

	/* Round 2 */
    for (count = 20; count < 40; ++count)
        SHA1_20_39(count, buff, data, A, B, C, D, E);

	/* Round 3 */
    for (count = 40; count < 60; ++count)
        SHA1_40_59(count, buff, data, A, B, C, D, E);

	/* Round 4 */
    for (count = 60; count < 80; ++count)
        SHA1_60_79(count, buff, data, A, B, C, D, E);

    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
}
EXPORT_SYMBOL(sha1_transform);

void sha1_init(uint32_t *digest)
{
    digest[0] = SHA1_DIGEST0;
    digest[1] = SHA1_DIGEST1;
    digest[2] = SHA1_DIGEST2;
    digest[3] = SHA1_DIGEST3;
    digest[4] = SHA1_DIGEST4;
}
EXPORT_SYMBOL(sha1_init);
