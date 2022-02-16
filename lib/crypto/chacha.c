/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto.h>
#include <string.h>
#include <bitops.h>
#include <proc.h>
#include <export.h>

static void chacha_permute(uint32_t *block, unsigned int rounds)
{
    unsigned int count;

    for (count = 0; count < rounds; count += 2) {
        block[0]  += block[4];    block[12] = rol32(block[12] ^ block[0],  16);
        block[1]  += block[5];    block[13] = rol32(block[13] ^ block[1],  16);
        block[2]  += block[6];    block[14] = rol32(block[14] ^ block[2],  16);
        block[3]  += block[7];    block[15] = rol32(block[15] ^ block[3],  16);

        block[8]  += block[12];   block[4]  = rol32(block[4]  ^ block[8],  12);
        block[9]  += block[13];   block[5]  = rol32(block[5]  ^ block[9],  12);
        block[10] += block[14];   block[6]  = rol32(block[6]  ^ block[10], 12);
        block[11] += block[15];   block[7]  = rol32(block[7]  ^ block[11], 12);

        block[0]  += block[4];    block[12] = rol32(block[12] ^ block[0],   8);
        block[1]  += block[5];    block[13] = rol32(block[13] ^ block[1],   8);
        block[2]  += block[6];    block[14] = rol32(block[14] ^ block[2],   8);
        block[3]  += block[7];    block[15] = rol32(block[15] ^ block[3],   8);

        block[8]  += block[12];   block[4]  = rol32(block[4]  ^ block[8],   7);
        block[9]  += block[13];   block[5]  = rol32(block[5]  ^ block[9],   7);
        block[10] += block[14];   block[6]  = rol32(block[6]  ^ block[10],  7);
        block[11] += block[15];   block[7]  = rol32(block[7]  ^ block[11],  7);

        block[0]  += block[5];    block[15] = rol32(block[15] ^ block[0],  16);
        block[1]  += block[6];    block[12] = rol32(block[12] ^ block[1],  16);
        block[2]  += block[7];    block[13] = rol32(block[13] ^ block[2],  16);
        block[3]  += block[4];    block[14] = rol32(block[14] ^ block[3],  16);

        block[10] += block[15];   block[5]  = rol32(block[5]  ^ block[10], 12);
        block[11] += block[12];   block[6]  = rol32(block[6]  ^ block[11], 12);
        block[8]  += block[13];   block[7]  = rol32(block[7]  ^ block[8],  12);
        block[9]  += block[14];   block[4]  = rol32(block[4]  ^ block[9],  12);

        block[0]  += block[5];    block[15] = rol32(block[15] ^ block[0],   8);
        block[1]  += block[6];    block[12] = rol32(block[12] ^ block[1],   8);
        block[2]  += block[7];    block[13] = rol32(block[13] ^ block[2],   8);
        block[3]  += block[4];    block[14] = rol32(block[14] ^ block[3],   8);

        block[10] += block[15];   block[5]  = rol32(block[5]  ^ block[10],  7);
        block[11] += block[12];   block[6]  = rol32(block[6]  ^ block[11],  7);
        block[8]  += block[13];   block[7]  = rol32(block[7]  ^ block[8],   7);
        block[9]  += block[14];   block[4]  = rol32(block[4]  ^ block[9],   7);
    }
}

void hchacha_block(uint32_t *state, uint8_t *stream, unsigned int rounds)
{
    uint32_t block[16];

    memcpy(block, state, 64);
    chacha_permute(block, rounds);

    memcpy(&stream[0], &block[0], 16);
    memcpy(&stream[4], &block[12], 16);
}
EXPORT_SYMBOL(hchacha_block);

void chacha_block(uint32_t *state, uint8_t *stream, unsigned int rounds)
{
    unsigned int count;
    uint32_t block[16];

    memcpy(block, state, 64);
    chacha_permute(block, rounds);

    for (count = 0; count < ARRAY_SIZE(block); ++count) {
        unaligned_set_le32(
            &stream[count * sizeof(uint32_t)],
            block[count] + state[count]
        );
    }

    ++block[12];
}
EXPORT_SYMBOL(chacha_block);

void chacha_init_const(uint32_t *state)
{
    state[0]  = 0x61707865; /* "expa" */
    state[1]  = 0x3320646e; /* "nd 3" */
    state[2]  = 0x79622d32; /* "2-by" */
    state[3]  = 0x6b206574; /* "te k" */
}
EXPORT_SYMBOL(chacha_init_const);

void chacha_init(uint32_t *state, const uint32_t *key, const uint8_t *iv)
{
    chacha_init_const(state);
    state[4]  = key[0];
    state[5]  = key[1];
    state[6]  = key[2];
    state[7]  = key[3];
    state[8]  = key[4];
    state[9]  = key[5];
    state[10] = key[6];
    state[11] = key[7];
    state[12] = unaligned_get_le32(iv +  0);
    state[13] = unaligned_get_le32(iv +  4);
    state[14] = unaligned_get_le32(iv +  8);
    state[15] = unaligned_get_le32(iv + 12);
}
EXPORT_SYMBOL(chacha_init);
