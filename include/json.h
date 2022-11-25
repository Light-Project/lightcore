/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _JSON_H_
#define _JSON_H_

#include <list.h>
#include <kmalloc.h>
#include <bitflags.h>

enum json_flags {
    __JSON_IS_ARRAY     = 0,
    __JSON_IS_OBJECT    = 1,
    __JSON_IS_STRING    = 2,
    __JSON_IS_NUMBER    = 3,
    __JSON_IS_NULL      = 4,
    __JSON_IS_TRUE      = 5,
    __JSON_IS_FALSE     = 6,
};

#define JSON_IS_ARRAY   BIT(__JSON_IS_ARRAY)
#define JSON_IS_OBJECT  BIT(__JSON_IS_OBJECT)
#define JSON_IS_STRING  BIT(__JSON_IS_STRING)
#define JSON_IS_NUMBER  BIT(__JSON_IS_NUMBER)
#define JSON_IS_NULL    BIT(__JSON_IS_NULL)
#define JSON_IS_TRUE    BIT(__JSON_IS_TRUE)
#define JSON_IS_FALSE   BIT(__JSON_IS_FALSE)

struct json_node {
    struct json_node *parent;
    struct list_head sibling;
    const char *name;
    unsigned long flags;
    union {
        struct list_head child;
        long number;
        char *string;
    };
};

GENERIC_STRUCT_BITOPS(json, struct json_node, flags, false)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, array, __JSON_IS_ARRAY)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, object, __JSON_IS_OBJECT)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, string, __JSON_IS_STRING)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, number, __JSON_IS_NUMBER)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, null, __JSON_IS_NULL)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, true, __JSON_IS_TRUE)
GENERIC_STRUCT_FLAG(json, struct json_node, flags, false, __JSON_IS_FALSE)
extern state json_parse(const char *buff, struct json_node **root, gfp_t gfp);
extern int json_encode(struct json_node *root, char *buff, int size);
extern void json_release(struct json_node *root);

#endif  /* _JSON_H_ */
