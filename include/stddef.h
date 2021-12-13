/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDDEF_H_
#define _STDDEF_H_

enum bool {
    false = 0,
    true = 1,
};

#undef NULL
#define NULL ((void *)0)

#undef offsetof
#ifdef __compiler_offsetof
# define offsetof(TYPE, MEMBER)	__compiler_offsetof(TYPE, MEMBER)
#else
# define offsetof(type, field) ((size_t)(&((type *)0)->field))
#endif

/**
 * sizeof_field - Report the size of a struct field in bytes.
 * @type: the structure containing the field of interest.
 * @member: the field to return the size of.
 */
#define sizeof_field(type, member) sizeof((((type *)0)->member))

/**
 * offsetofend - Report the offset of a struct field within the struct.
 * @type: the type of the structure.
 * @member: the member within the structure to get the end offset of.
 */
#define offsetofend(type, member) (offsetof(type, member) + sizeof_field(type, member))

#endif  /* _STDDEF_H_ */
