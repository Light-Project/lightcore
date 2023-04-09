/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/sha256.h>
#include <bitops.h>
#include <proc.h>
#include <export.h>

static const uint32_t sha256_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

#define S0(x) (ror32(x,  7) ^ ror32(x, 18) ^ (x >>  3))
#define S1(x) (ror32(x, 17) ^ ror32(x, 19) ^ (x >> 10))
#define E0(x) (ror32(x,  2) ^ ror32(x, 13) ^ ror32(x, 22))
#define E1(x) (ror32(x,  6) ^ ror32(x, 11) ^ ror32(x, 25))
#define CH(x, y, z) (z ^ (x & (y ^ z)))
#define MAJ(x, y, z) ((x & y) | (z & (x | y)))

#define SHA256_LOAD(c, b, d) (b[c] = unaligned_get_be32((uint32_t *)d + c))
#define SHA256_BLEND(c, b) (b[c] = S1(b[c - 2]) + b[c - 7] + S0(b[c - 15]) + b[c - 16])
#define SHA256_ROUND(c, b, A, B, C, D, E, F, G, H) do {     \
    uint32_t tmp1, tmp2;                                    \
    tmp1 = H + E1(E) + CH(E, F, G) + sha256_k[c] + b[c];    \
    tmp2 = E0(A) + MAJ(A, B, C);                            \
    D += tmp1; H = tmp1 + tmp2;                             \
} while (0)

void sha256_transform(uint32_t *digest, uint32_t *buff, const char *data)
{
    uint32_t A, B, C, D, E, F, G, H;
    unsigned int count;

    /* load state */
    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];
    F = digest[5];
    G = digest[6];
    H = digest[7];

    /* load data */
    for (count = 0; count < 16; count += 8) {
        SHA256_LOAD(count + 0, buff, data);
        SHA256_LOAD(count + 1, buff, data);
        SHA256_LOAD(count + 2, buff, data);
        SHA256_LOAD(count + 3, buff, data);
        SHA256_LOAD(count + 4, buff, data);
        SHA256_LOAD(count + 5, buff, data);
        SHA256_LOAD(count + 6, buff, data);
        SHA256_LOAD(count + 7, buff, data);
    }

    /* blend data */
    for (count = 16; count < 64; count += 8) {
        SHA256_BLEND(count + 0, buff);
        SHA256_BLEND(count + 1, buff);
        SHA256_BLEND(count + 2, buff);
        SHA256_BLEND(count + 3, buff);
        SHA256_BLEND(count + 4, buff);
        SHA256_BLEND(count + 5, buff);
        SHA256_BLEND(count + 6, buff);
        SHA256_BLEND(count + 7, buff);
    }

    /* iterate register */
    for (count = 0; count < 64; count += 8) {
        SHA256_ROUND(count + 0, buff, A, B, C, D, E, F, G, H);
        SHA256_ROUND(count + 1, buff, H, A, B, C, D, E, F, G);
        SHA256_ROUND(count + 2, buff, G, H, A, B, C, D, E, F);
        SHA256_ROUND(count + 3, buff, F, G, H, A, B, C, D, E);
        SHA256_ROUND(count + 4, buff, E, F, G, H, A, B, C, D);
        SHA256_ROUND(count + 5, buff, D, E, F, G, H, A, B, C);
        SHA256_ROUND(count + 6, buff, C, D, E, F, G, H, A, B);
        SHA256_ROUND(count + 7, buff, B, C, D, E, F, G, H, A);
    }

    /* update state */
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
    digest[5] += F;
    digest[6] += G;
    digest[7] += H;
}
EXPORT_SYMBOL(sha256_transform);

void sha224_transform(uint32_t *digest, uint32_t *buff, const char *data)
{
    sha256_transform(digest, buff, data);
}
EXPORT_SYMBOL(sha224_transform);

void sha256_init(uint32_t *digest)
{
    digest[0] = SHA256_DIGEST0;
    digest[1] = SHA256_DIGEST1;
    digest[2] = SHA256_DIGEST2;
    digest[3] = SHA256_DIGEST3;
    digest[4] = SHA256_DIGEST4;
    digest[5] = SHA256_DIGEST5;
    digest[6] = SHA256_DIGEST6;
    digest[7] = SHA256_DIGEST7;
}
EXPORT_SYMBOL(sha256_init);

void sha224_init(uint32_t *digest)
{
    digest[0] = SHA224_DIGEST0;
    digest[1] = SHA224_DIGEST1;
    digest[2] = SHA224_DIGEST2;
    digest[3] = SHA224_DIGEST3;
    digest[4] = SHA224_DIGEST4;
    digest[5] = SHA224_DIGEST5;
    digest[6] = SHA224_DIGEST6;
    digest[7] = SHA224_DIGEST7;
}
EXPORT_SYMBOL(sha224_init);
