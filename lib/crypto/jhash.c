/*
 * https://burtleburtle.net/bob/hash/
 * These are the credits from Bob's sources:
 * lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 *
 * Copyright (C) 2006. Bob Jenkins (bob_jenkins@burtleburtle.net)
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/jhash.h>
#include <proc.h>
#include <export.h>

static __always_inline uint32_t
__jhash(const uint8_t *data, size_t length, uint32_t initval)
{
    uint32_t a, b, c;

    /* Set up the internal state */
    a = b = c = JHASH_INITVAL + length + initval;

    /* All but the last block: affect some 32 bits of (a,b,c) */
    for (; length > 12; length -= 12, data += 12) {
        a += unaligned_get_u32(data);
        b += unaligned_get_u32(data + 4);
        c += unaligned_get_u32(data + 8);
        jhash_mix(a, b, c);
    }

    /* Last block: affect all 32 bits of (c) */
    switch (length) {
        case 12: c += (uint32_t)data[11] << 24; fallthrough;
        case 11: c += (uint32_t)data[10] << 16; fallthrough;
        case 10: c += (uint32_t)data[ 9] <<  8; fallthrough;
        case  9: c += (uint32_t)data[ 8] <<  0; fallthrough;
        case  8: b += (uint32_t)data[ 7] << 24; fallthrough;
        case  7: b += (uint32_t)data[ 6] << 16; fallthrough;
        case  6: b += (uint32_t)data[ 5] <<  8; fallthrough;
        case  5: b += (uint32_t)data[ 4] <<  0; fallthrough;
        case  4: a += (uint32_t)data[ 3] << 24; fallthrough;
        case  3: a += (uint32_t)data[ 2] << 16; fallthrough;
        case  2: a += (uint32_t)data[ 1] <<  8; fallthrough;
        case  1: a += (uint32_t)data[ 0] <<  0;
            jhash_final(a, b, c);
            break;

        /* Nothing left to add */
        default:
            break;
    }

    return c;
}

uint32_t jhash(const void *data, size_t length, uint32_t initval)
{
    return __jhash(data, length, initval);
}
EXPORT_SYMBOL(jhash);
