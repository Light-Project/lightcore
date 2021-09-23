/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDDEF_H_
#define _STDDEF_H_

enum bool {
    false = 0,
    true = 1,
};

#define NULL ((void *)0)

#define offsetof(type, field) ((size_t)(&((type *)0)->field))
        
/**
 * sizeof_field(TYPE, MEMBER)
 *
 * @TYPE: The structure containing the field of interest
 * @MEMBER: The field to return the size of
 */
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

/**
 * offsetofend(TYPE, MEMBER)
 *
 * @TYPE: The type of the structure
 * @MEMBER: The member within the structure to get the end offset of
 */
#define offsetofend(TYPE, MEMBER) (offsetof(TYPE, MEMBER) + sizeof_field(TYPE, MEMBER))

#endif /* __STDDEF_H__ */
