/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/base64.h>
#include <export.h>

static const char base64_encode_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

static const char base64_decode_table[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static __always_inline void
__base64_encode(uint8_t *buff, const uint8_t *data, size_t size)
{
    const uint8_t *prev = data;
    unsigned int bstate = 0;

    for (; size >= 3; size -= 3) {
        *buff++ = base64_encode_table[*data++ >> 2];
        *buff++ = base64_encode_table[((*prev++ & 0x3) << 4) | (*data++ >> 4)];
        *buff++ = base64_encode_table[((*prev++ & 0xf) << 2) | (*data++ >> 6)];
        *buff++ = base64_encode_table[*prev++ & 0x3f];
    }

    while (size--) switch (bstate) {
        case 0:
            *buff++ = base64_encode_table[*data++ >> 2];
            bstate = 1;
            break;

        case 1:
            *buff++ = base64_encode_table[((*prev++ & 0x3) << 4) | (*data++ >> 4)];
            bstate = 2;
            break;

        default:
            *buff++ = base64_encode_table[((*prev++ & 0xf) << 2) | (*data++ >> 6)];
            *buff++ = base64_encode_table[*prev++ & 0x3f];
            bstate = 0;
            break;
    }

    switch (bstate) {
        case 1:
            *buff++ = base64_encode_table[(*prev & 0x3) << 4];
            *buff++ = '=';
            *buff++ = '=';
            break;

        case 2:
            *buff++ = base64_encode_table[(*prev & 0xf) << 2];
            *buff++ = '=';
            break;

        default:
            break;
    }

    *buff = '\0';
}

static __always_inline state
__base64_decode(uint8_t *buff, const uint8_t *data, size_t size)
{
    unsigned int bstate = 0;
    uint8_t decode;

    while (size--) {
        if ((*data < '+') || (*data > 'z'))
            return -EINVAL;
        else if (*data == '=')
            break;

        decode = base64_decode_table[*data++];
        if (decode == 0xff)
            return -EINVAL;

        switch (bstate++ & 0x3) {
            case 0:
                *buff = decode << 2;
                break;

            case 1:
                *buff++ |= (decode >> 4) & 0x3;
                *buff = decode << 4;
                break;

            case 2:
                *buff++ |= (decode >> 2) & 0xf;
                *buff = decode << 6;
                break;

            default:
                *buff++ |= decode;
                break;
        }
    }

    return -ENOERR;
}

void base64_encode(void *buff, const void *data, size_t size)
{
    __base64_encode(buff, data, size);
}
EXPORT_SYMBOL(base64_encode);

state base64_decode(void *buff, const void *data, size_t size)
{
    return __base64_decode(buff, data, size);
}
EXPORT_SYMBOL(base64_decode);
