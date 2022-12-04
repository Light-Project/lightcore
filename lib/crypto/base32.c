/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/base32.h>
#include <export.h>

static const char base32_encode_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '2', '3', '4', '5', '6', '7',
};

static const char base32_decode_table[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static __always_inline void
__base32_encode(uint8_t *buff, const uint8_t *data, size_t size)
{
    const uint8_t *prev = data;
    unsigned int bstate = 0;

    for (; size >= 5; size -= 5) {
        *buff++ = base32_encode_table[*data++ >> 3];
        *buff++ = base32_encode_table[((*prev++ & 0x7) << 2) | (*data++ >> 6)];
        *buff++ = base32_encode_table[(*prev >> 1) & 0x1f];
        *buff++ = base32_encode_table[((*prev++ & 0x1) << 4) | (*data++ >> 4)];
        *buff++ = base32_encode_table[((*prev++ & 0xf) << 1) | (*data++ >> 7)];
        *buff++ = base32_encode_table[(*prev >> 2) & 0x1f];
        *buff++ = base32_encode_table[((*prev++ & 0x3) << 3) | (*data++ >> 5)];
        *buff++ = base32_encode_table[*prev++ & 0x1f];
    }

    while (size--) switch (bstate) {
        case 0:
            *buff++ = base32_encode_table[*data++ >> 3];
            bstate = 1;
            break;

        case 1:
            *buff++ = base32_encode_table[((*prev++ & 0x7) << 2) | (*data++ >> 6)];
            *buff++ = base32_encode_table[(*prev >> 1) & 0x1f];
            bstate = 2;
            break;

        case 2:
            *buff++ = base32_encode_table[((*prev++ & 0x1) << 4) | (*data++ >> 4)];
            bstate = 3;
            break;

        case 3:
            *buff++ = base32_encode_table[((*prev++ & 0xf) << 1) | (*data++ >> 7)];
            *buff++ = base32_encode_table[(*prev >> 2) & 0x1f];
            bstate = 4;
            break;

        default:
            *buff++ = base32_encode_table[((*prev++ & 0x3) << 3) | (*data++ >> 5)];
            *buff++ = base32_encode_table[*prev++ & 0x1f];
            bstate = 0;
            break;
    }

    switch (bstate) {
        case 1:
            *buff++ = base32_encode_table[(*prev & 0x7) << 2];
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            break;

        case 2:
            *buff++ = base32_encode_table[(*prev & 0x1) << 4];
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            break;

        case 3:
            *buff++ = base32_encode_table[(*prev & 0xf) << 1];
            *buff++ = '=';
            *buff++ = '=';
            *buff++ = '=';
            break;

        case 4:
            *buff++ = base32_encode_table[(*prev & 0x3) << 3];
            *buff++ = '=';
            break;

        default:
            break;
    }

    *buff = '\0';
}

static __always_inline state
__base32_decode(uint8_t *buff, const uint8_t *data, size_t size)
{
    unsigned int bstate = 0;
    uint8_t decode;

    while (size--) {
        if ((*data < '2') || (*data > 'Z'))
            return -EINVAL;
        else if (*data == '=')
            break;

        decode = base32_decode_table[*data++];
        if (decode == 0xff)
            return -EINVAL;

        switch (bstate++ & 0x7) {
            case 0:
                *buff = decode << 3;
                break;

            case 1:
                *buff++ |= (decode >> 2) & 0x7;
                *buff = decode << 6;
                break;

            case 2:
                *buff |= (decode & 0x1f) << 1;
                break;

            case 3:
                *buff++ |= (decode >> 4) & 0x1;
                *buff = decode << 4;
                break;

            case 4:
                *buff++ |= (decode >> 1) & 0xf;
                *buff = decode << 7;
                break;

            case 5:
                *buff |= (decode & 0x1f) << 2;
                break;

            case 6:
                *buff++ |= (decode >> 3) & 0x3;
                *buff = decode << 5;
                break;

            default:
                *buff++ |= decode & 0x1f;
                break;
        }
    }

    return -ENOERR;
}

void base32_encode(void *buff, const void *data, size_t size)
{
    __base32_encode(buff, data, size);
}
EXPORT_SYMBOL(base32_encode);

state base32_decode(void *buff, const void *data, size_t size)
{
    return __base32_decode(buff, data, size);
}
EXPORT_SYMBOL(base32_decode);
