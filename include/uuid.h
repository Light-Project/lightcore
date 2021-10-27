/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _UUID_H_
#define _UUID_H_

#include <string.h>
#include <state.h>
#include <lightcore/uuid.h>

#define UUID_SIZE 16

typedef struct {
    __u8 b[UUID_SIZE];
} uuid_t;

#define UUID_STRING_LEN 36

bool uuid_check(const char *uuid);

#endif  /* _UUID_H_ */
