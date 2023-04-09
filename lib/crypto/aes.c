/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/aes.h>
#include <bitops.h>
#include <proc.h>
#include <export.h>

static const uint8_t aes_sbox[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

static const uint8_t aes_inv_sbox[] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

static __always_inline uint32_t mulx(uint32_t value)
{
    uint32_t x, y;
    x = value & 0x7f7f7f7f;
    y = value & 0x80808080;
    return (x << 1) ^ (y >> 7) * 0x1b;
}

static __always_inline uint32_t mulx2(uint32_t value)
{
    uint32_t x, y, z;
    x = value & 0x3f3f3f3f;
    y = value & 0x80808080;
    z = value & 0x40404040;
    return (x << 2) ^ (y >> 7) * 0x36 ^ (z >> 6) * 0x1b;
}

static __always_inline uint32_t mixcol(uint32_t x)
{
    uint32_t y = mulx(x) ^ ror32(x, 16);
    return y ^ ror32(x ^ y, 8);
}

static __always_inline uint32_t invcol(uint32_t x)
{
    uint32_t y = mulx2(x);
    return mixcol(x ^ y ^ ror32(y, 16));
}

static __always_inline uint32_t subshift(uint32_t *src, int pos)
{
    return (aes_sbox[src[pos] & 0xff]) ^
           (aes_sbox[(src[(pos + 1) % 4] >>  8) & 0xff] <<  8) ^
           (aes_sbox[(src[(pos + 2) % 4] >> 16) & 0xff] << 16) ^
           (aes_sbox[(src[(pos + 3) % 4] >> 24) & 0xff] << 24);
}

static __always_inline uint32_t invshift(uint32_t *src, int pos)
{
    return (aes_inv_sbox[src[pos] & 0xff]) ^
           (aes_inv_sbox[(src[(pos + 3) % 4] >>  8) & 0xff] <<  8) ^
           (aes_inv_sbox[(src[(pos + 2) % 4] >> 16) & 0xff] << 16) ^
           (aes_inv_sbox[(src[(pos + 1) % 4] >> 24) & 0xff] << 24);
}

static __always_inline uint32_t subw(uint32_t value)
{
    return (aes_sbox[value & 0xff]) ^
           (aes_sbox[(value >>  8) & 0xff] <<  8) ^
           (aes_sbox[(value >> 16) & 0xff] << 16) ^
           (aes_sbox[(value >> 24) & 0xff] << 24);
}

void aes_encrypt(struct aes_context *ctx, const uint8_t *src, uint8_t *dest)
{
    uint32_t buff[8], *enkey = ctx->encrypt_key + 4;
    unsigned int count;

    buff[0] = ctx->encrypt_key[0] ^ unaligned_get_le32(src +  0);
    buff[1] = ctx->encrypt_key[1] ^ unaligned_get_le32(src +  4);
    buff[2] = ctx->encrypt_key[2] ^ unaligned_get_le32(src +  8);
    buff[3] = ctx->encrypt_key[3] ^ unaligned_get_le32(src + 12);

    buff[0] ^= aes_sbox[ 0] ^ aes_sbox[ 64] ^ aes_sbox[134] ^ aes_sbox[195];
    buff[1] ^= aes_sbox[16] ^ aes_sbox[ 82] ^ aes_sbox[158] ^ aes_sbox[221];
    buff[2] ^= aes_sbox[32] ^ aes_sbox[ 96] ^ aes_sbox[160] ^ aes_sbox[234];
    buff[3] ^= aes_sbox[48] ^ aes_sbox[112] ^ aes_sbox[186] ^ aes_sbox[241];

    for (count = 0;; count += 2, enkey += 8) {
        buff[4] = mixcol(subshift(buff + 0, 0)) ^ enkey[0];
        buff[5] = mixcol(subshift(buff + 0, 1)) ^ enkey[1];
        buff[6] = mixcol(subshift(buff + 0, 2)) ^ enkey[2];
        buff[7] = mixcol(subshift(buff + 0, 3)) ^ enkey[3];

        if (count >= ctx->keylen / 4 + 4)
            break;

        buff[0] = mixcol(subshift(buff + 4, 0)) ^ enkey[4];
        buff[1] = mixcol(subshift(buff + 4, 1)) ^ enkey[5];
        buff[2] = mixcol(subshift(buff + 4, 2)) ^ enkey[6];
        buff[3] = mixcol(subshift(buff + 4, 3)) ^ enkey[7];
    }

    unaligned_set_le32(dest +  0, subshift(buff + 4, 0) ^ enkey[4]);
    unaligned_set_le32(dest +  4, subshift(buff + 4, 1) ^ enkey[5]);
    unaligned_set_le32(dest +  8, subshift(buff + 4, 2) ^ enkey[6]);
    unaligned_set_le32(dest + 12, subshift(buff + 4, 3) ^ enkey[7]);
}
EXPORT_SYMBOL(aes_encrypt);

void aes_decrypt(struct aes_context *ctx, const uint8_t *src, uint8_t *dest)
{
    uint32_t buff[8], *enkey = ctx->encrypt_key + 4;
    unsigned int count;

    buff[0] = ctx->decrypt_key[0] ^ unaligned_get_le32(src +  0);
    buff[1] = ctx->decrypt_key[1] ^ unaligned_get_le32(src +  4);
    buff[2] = ctx->decrypt_key[2] ^ unaligned_get_le32(src +  8);
    buff[3] = ctx->decrypt_key[3] ^ unaligned_get_le32(src + 12);

    buff[0] ^= aes_inv_sbox[ 0] ^ aes_inv_sbox[ 64] ^ aes_inv_sbox[129] ^ aes_inv_sbox[200];
    buff[1] ^= aes_inv_sbox[16] ^ aes_inv_sbox[ 83] ^ aes_inv_sbox[150] ^ aes_inv_sbox[212];
    buff[2] ^= aes_inv_sbox[32] ^ aes_inv_sbox[ 96] ^ aes_inv_sbox[160] ^ aes_inv_sbox[236];
    buff[3] ^= aes_inv_sbox[48] ^ aes_inv_sbox[112] ^ aes_inv_sbox[187] ^ aes_inv_sbox[247];

    for (count = 0;; count += 2, enkey += 8) {
        buff[4] = invcol(invshift(buff + 0, 0)) ^ enkey[0];
        buff[5] = invcol(invshift(buff + 0, 1)) ^ enkey[1];
        buff[6] = invcol(invshift(buff + 0, 2)) ^ enkey[2];
        buff[7] = invcol(invshift(buff + 0, 3)) ^ enkey[3];

        if (count >= ctx->keylen / 4 + 4)
            break;

        buff[0] = invcol(invshift(buff + 4, 0)) ^ enkey[4];
        buff[1] = invcol(invshift(buff + 4, 1)) ^ enkey[5];
        buff[2] = invcol(invshift(buff + 4, 2)) ^ enkey[6];
        buff[3] = invcol(invshift(buff + 4, 3)) ^ enkey[7];
    }

    unaligned_set_le32(dest +  0, invshift(buff + 4, 0) ^ enkey[4]);
    unaligned_set_le32(dest +  4, invshift(buff + 4, 1) ^ enkey[5]);
    unaligned_set_le32(dest +  8, invshift(buff + 4, 2) ^ enkey[6]);
    unaligned_set_le32(dest + 12, invshift(buff + 4, 3) ^ enkey[7]);
}
EXPORT_SYMBOL(aes_decrypt);

state aes_expandkey(struct aes_context *ctx, const uint8_t *inkey, unsigned int keylen)
{
    unsigned int words = keylen / BYTES_PER_U32;
    unsigned int count, index;
    uint32_t rc;

    if (!aes_check_keylen(keylen))
        return -EINVAL;

    ctx->keylen = keylen;

    for (count = 0; count < words; ++count) {
        ctx->encrypt_key[count] =
        unaligned_get_le32(inkey + count * BYTES_PER_U32);
    }

    for (count = 0, rc = 1; count < 10; ++count) {
        uint32_t *rki, *rko;

        rki = ctx->encrypt_key + (count * words);
        rko = rki + words;

        rko[0] = ror32(subw(rki[words - 1]), 8) ^ rc ^ rki[0];
        rko[1] = rko[0] ^ rki[1];
        rko[2] = rko[1] ^ rki[2];
        rko[3] = rko[2] ^ rki[3];

        switch (keylen) {
            case AES_KEYSIZE_192:
                if (count >= 7)
                    break;
                rko[4] = rko[3] ^ rki[4];
                rko[5] = rko[4] ^ rki[5];
                break;

            case AES_KEYSIZE_256:
                if (count >= 6)
                    break;
                rko[4] = subw(rko[3]) ^ rki[4];
                rko[5] = rko[4] ^ rki[5];
                rko[6] = rko[5] ^ rki[6];
                rko[7] = rko[6] ^ rki[7];
                break;

            default:
                break;
        }

        rc = mulx(rc);
    }

    ctx->decrypt_key[0] = ctx->encrypt_key[keylen + 24];
    ctx->decrypt_key[1] = ctx->encrypt_key[keylen + 25];
    ctx->decrypt_key[2] = ctx->encrypt_key[keylen + 26];
    ctx->decrypt_key[3] = ctx->encrypt_key[keylen + 27];

    for (count = keylen + 20, index = 4; count > 0; count -= 4, index += 4) {
        ctx->decrypt_key[index + 0] = invcol(ctx->encrypt_key[count + 0]);
        ctx->decrypt_key[index + 1] = invcol(ctx->encrypt_key[count + 1]);
        ctx->decrypt_key[index + 2] = invcol(ctx->encrypt_key[count + 2]);
        ctx->decrypt_key[index + 3] = invcol(ctx->encrypt_key[count + 3]);
    }

    ctx->decrypt_key[count + 0] = ctx->encrypt_key[0];
    ctx->decrypt_key[count + 1] = ctx->encrypt_key[1];
    ctx->decrypt_key[count + 2] = ctx->encrypt_key[2];
    ctx->decrypt_key[count + 3] = ctx->encrypt_key[3];

    return -ENOERR;
}
EXPORT_SYMBOL(aes_expandkey);
