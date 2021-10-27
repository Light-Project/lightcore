/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <ctype.h>
#include <random.h>
#include <uuid.h>
#include <export.h>

const uint8_t guid_index[GUID_SIZE] = {
    3, 2, 1, 0, 5, 4, 7, 6, 8,
    9, 10, 11, 12, 13, 14, 15,
};

const uint8_t uuid_index[UUID_SIZE] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15,
};

void generate_random_uuid(unsigned char uuid[16])
{
    random_get(uuid, 16);
    /* Set UUID version to 4 --- truly random generation */
    uuid[6] = (uuid[6] & 0x0F) | 0x40;
    /* Set the UUID variant to DCE */
    uuid[8] = (uuid[8] & 0x3F) | 0x80;
}

void generate_random_guid(unsigned char guid[16])
{
    random_get(guid, 16);
    /* Set GUID version to 4 --- truly random generation */
    guid[7] = (guid[7] & 0x0F) | 0x40;
    /* Set the GUID variant to DCE */
    guid[8] = (guid[8] & 0x3F) | 0x80;
}

static void __uuid_gen_common(__u8 b[16])
{
    prandom_bytes(b, 16);
    /* reversion 0b10 */
    b[8] = (b[8] & 0x3F) | 0b10;
}

void guid_gen(guid_t *lu)
{
    __uuid_gen_common(lu->b);
    /* version 4 : random generation */
    lu->b[7] = (lu->b[7] & 0x0F) | 0x40;
}

void uuid_gen(uuid_t *bu)
{
    __uuid_gen_common(bu->b);
    /* version 4 : random generation */
    bu->b[6] = (bu->b[6] & 0x0F) | 0x40;
}

bool uuid_check(const char *uuid)
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

static int __uuid_parse(const char *uuid, __u8 b[16], const u8 ei[16])
{
    static const u8 si[16] = {0,2,4,6,9,11,14,16,19,21,24,26,28,30,32,34};
    unsigned int i;

    if (!uuid_is_valid(uuid))
        return -EINVAL;

    for (i = 0; i < 16; i++) {
        int hi = hex_to_bin(uuid[si[i] + 0]);
        int lo = hex_to_bin(uuid[si[i] + 1]);

        b[ei[i]] = (hi << 4) | lo;
    }

    return 0;
}

int guid_parse(const char *uuid, guid_t *u)
{
    return __uuid_parse(uuid, u->b, guid_index);
}

int uuid_parse(const char *uuid, uuid_t *u)
{
    return __uuid_parse(uuid, u->b, uuid_index);
}

EXPORT_SYMBOL(generate_random_guid);
EXPORT_SYMBOL(guid_gen);
EXPORT_SYMBOL(uuid_gen);
EXPORT_SYMBOL(uuid_check);
EXPORT_SYMBOL(guid_parse);
EXPORT_SYMBOL(uuid_parse);
