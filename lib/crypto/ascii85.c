/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/ascii85.h>
#include <minmax.h>
#include <proc.h>
#include <math.h>
#include <string.h>
#include <export.h>

static __always_inline void
__ascii85_encode(char *buff, const void *data, size_t size)
{
    uint32_t value;

    for (; size >= 4; size -= 4) {
        value = unaligned_get_be32(data);
        data += 4;

        if (value == 0)
            *buff++ = 'z';
        else {
            buff[4] = '!' + value % 85; value /= 85;
            buff[3] = '!' + value % 85; value /= 85;
            buff[2] = '!' + value % 85; value /= 85;
            buff[1] = '!' + value % 85; value /= 85;
            buff[0] = '!' + value % 85;
            buff += 5;
        }
    }

    if (size) {
        value = unaligned_get_be32(data);
        value &= BIT_HIGH_MASK((BYTES_PER_U32 - size) * BITS_PER_BYTE);

        if (value == 0)
            *buff++ = 'z';
        else {
            buff[4] = '!' + value % 85; value /= 85;
            buff[3] = '!' + value % 85; value /= 85;
            buff[2] = '!' + value % 85; value /= 85;
            buff[1] = '!' + value % 85; value /= 85;
            buff[0] = '!' + value % 85;
            buff += 5;
        }
    }

    *buff = '\0';
}

static __always_inline state
__ascii85_decode(void *buff, const char *data, size_t size)
{
    unsigned int index;
    uint32_t value;

    while (size) {
        if (*data == 'z' && size >= 1) {
            memset(buff, 0, 4);
            data += 1;
            size -= 1;
        } else if (size >= 5) {
            for (index = 0; index < 5; ++index) {
                if (data[index] < '!' || 'u' < data[index])
                    return -EINVAL;
            }

            value  = data[0] - '!'; value *= 85;
            value += data[1] - '!'; value *= 85;
            value += data[2] - '!'; value *= 85;
            value += data[3] - '!'; value *= 85;
            value += data[4] - '!';
            unaligned_set_be32(buff, value);

            data += 5;
            size -= 5;
        } else
            return -EINVAL;

        buff += 4;
    }

    return -ENOERR;
}

static __always_inline size_t
__ascii85_encode_length(const uint32_t *data, size_t size)
{
    size_t csize, length;
    uint32_t value;

    for (length = 0; (csize = min(size, 4)); size -= csize) {
        value = unaligned_get_be32(data);
        value &= BIT_HIGH_MASK((BYTES_PER_U32 - size) * BITS_PER_BYTE);
        length += value ? 5 : 1;
        data++;
    }

    return length + 1;
}

static __always_inline size_t
__ascii85_decode_length(const char *data, size_t size)
{
    while ((data = strchr(data, 'z'))) {
        size += 4;
        data++;
    }

    return DIV_ROUND_UP(size, 5) * 4;
}

void ascii85_encode(void *buff, const void *data, size_t size)
{
    __ascii85_encode(buff, data, size);
}
EXPORT_SYMBOL(ascii85_encode);

state ascii85_decode(void *buff, const void *data, size_t size)
{
    return __ascii85_decode(buff, data, size);
}
EXPORT_SYMBOL(ascii85_decode);

size_t ascii85_encode_length(const void *data, size_t size)
{
    return __ascii85_encode_length(data, size);
}
EXPORT_SYMBOL(ascii85_encode_length);

size_t ascii85_decode_length(const void *data, size_t size)
{
    return __ascii85_decode_length(data, size);
}
EXPORT_SYMBOL(ascii85_decode_length);
