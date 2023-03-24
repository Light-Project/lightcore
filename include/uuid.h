/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _UUID_H_
#define _UUID_H_

#include <types.h>
#include <state.h>
#include <stddef.h>
#include <lightcore/uuid.h>

#define UUID_SIZE 16
#define UUID_STRING_LEN 36

typedef struct {
    uint8_t byte[UUID_SIZE];
} uuid_t;

extern const uint8_t guid_index[GUID_SIZE];
extern const uint8_t uuid_index[UUID_SIZE];

extern void guid_random_generate(unsigned char guid[16]);
extern void uuid_random_generate(unsigned char uuid[16]);
extern void guid_generate(guid_t *guid);
extern void uuid_generate(uuid_t *uuid);
extern void guid_string_generate(char *buff);
extern void uuid_string_generate(char *buff);
extern bool uuid_valid(const char *uuid);
extern state guid_parse(const char *str, guid_t *guid);
extern state uuid_parse(const char *str, uuid_t *uuid);

#endif /* _UUID_H_ */
