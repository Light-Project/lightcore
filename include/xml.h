/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _XML_H_
#define _XML_H_

#include <list.h>
#include <kmalloc.h>
#include <bitflags.h>

enum xml_flags {
    __XML_IS_OBJECT     = 0,
    __XML_IS_ATTRIBUTE  = 1,
    __XML_IS_STRING     = 2,
};

#define XML_IS_OBJECT       BIT(__XML_IS_OBJECT)
#define XML_IS_ATTRIBUTE    BIT(__XML_IS_ATTRIBUTE)
#define XML_IS_STRING       BIT(__XML_IS_STRING)

struct xml_node {
    struct xml_node *parent;
    struct list_head sibling;
    unsigned long flags;
    union {
        struct { /* object */
            char *name;
            struct list_head child;
        };
        struct { /* attribyte */
            char *attr_name;
            char *attr_value;
        }; /* string */
        char *string;
    };
};

GENERIC_STRUCT_BITOPS(xml, struct xml_node, flags, false)
GENERIC_STRUCT_FLAG(xml, struct xml_node, flags, object, __XML_IS_OBJECT)
GENERIC_STRUCT_FLAG(xml, struct xml_node, flags, attribute, __XML_IS_ATTRIBUTE)
GENERIC_STRUCT_FLAG(xml, struct xml_node, flags, string, __XML_IS_STRING)
extern state xml_parse(const char *buff, struct xml_node **root, gfp_t gfp);
extern int xml_encode(struct xml_node *root, char *buff, int size);
extern void xml_release(struct xml_node *root);

#endif  /* _XML_H_ */
