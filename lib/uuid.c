/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <uuid.h>
#include <ctype.h>
#include <kernel.h>
#include <prandom.h>
#include <export.h>

const uint8_t guid_index[GUID_SIZE] = {
    3, 2, 1, 0, 5, 4, 7, 6, 8,
    9, 10, 11, 12, 13, 14, 15,
};

const uint8_t uuid_index[UUID_SIZE] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15,
};

void guid_random_generate(unsigned char guid[16])
{
    prandom_bytes(guid, 16);
    guid[7] = (guid[7] & 0x0f) | 0x40;
    guid[8] = (guid[8] & 0x3f) | 0x80;
}
EXPORT_SYMBOL(guid_random_generate);

void uuid_random_generate(unsigned char uuid[16])
{
    prandom_bytes(uuid, 16);
    uuid[6] = (uuid[6] & 0x0f) | 0x40;
    uuid[8] = (uuid[8] & 0x3f) | 0x80;
}
EXPORT_SYMBOL(uuid_random_generate);

static void common_generate(uint8_t common[16])
{
    prandom_bytes(common, 16);
    common[8] = (common[8] & 0x3f) | 0x80;
}

void guid_generate(guid_t *guid)
{
    common_generate(guid->byte);
    guid->byte[7] = (guid->byte[7] & 0x0F) | 0x40;
}
EXPORT_SYMBOL(guid_generate);

void uuid_generate(uuid_t *uuid)
{
    common_generate(uuid->byte);
    uuid->byte[6] = (uuid->byte[6] & 0x0F) | 0x40;
}
EXPORT_SYMBOL(uuid_generate);

static void common_sprintf(char *buff, uint8_t uuid[16])
{
    unsigned int count;
    int offset = 0;

    for (count = 0; count < 16; ++count) {
        if (count == 4 || count == 6 ||
            count == 8 || count == 10)
            offset += sprintf(buff + offset, "-");
        offset += sprintf(buff + offset, "%02x", uuid[count]);
    }
}

void guid_string_generate(char *buff)
{
    guid_t guid;
    guid_generate(&guid);
    common_sprintf(buff, guid.byte);
}
EXPORT_SYMBOL(guid_string_generate);

void uuid_string_generate(char *buff)
{
    uuid_t uuid;
    uuid_generate(&uuid);
    common_sprintf(buff, uuid.byte);
}
EXPORT_SYMBOL(uuid_string_generate);

bool uuid_valid(const char *uuid)
{
    unsigned int i;

    for (i = 0; i < UUID_STRING_LEN; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (uuid[i] != '-')
                return false;
        } else if (!isxdigit(uuid[i])) {
            return false;
        }
    }

    return true;
}
EXPORT_SYMBOL(uuid_valid);

static state common_parse(const char *uuid, uint8_t byte[16], const u8 index[16])
{
    static const u8 si[16] = {0,2,4,6,9,11,14,16,19,21,24,26,28,30,32,34};
    unsigned int count;

    if (!uuid_valid(uuid))
        return -EINVAL;

    for (count = 0; count < 16; ++count) {
        int hi = chtoi(uuid[si[count] + 0]);
        int lo = chtoi(uuid[si[count] + 1]);
        byte[index[count]] = (hi << 4) | lo;
    }

    return -ENOERR;
}

state guid_parse(const char *str, guid_t *guid)
{
    return common_parse(str, guid->byte, guid_index);
}
EXPORT_SYMBOL(guid_parse);

state uuid_parse(const char *str, uuid_t *uuid)
{
    return common_parse(str, uuid->byte, uuid_index);
}
EXPORT_SYMBOL(uuid_parse);
