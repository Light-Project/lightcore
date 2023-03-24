/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifndef __ASSEMBLY__

enum bool {
    false   = 0,
    true    = 1,
};

#undef NULL
# define NULL ((void *)0)

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

/**
 * __struct_group - Create a mirrored named and anonyomous struct.
 * @TAG: The tag name for the named sub-struct (usually empty).
 * @NAME: The identifier name of the mirrored sub-struct.
 * @ATTRS: Any struct attributes (usually empty).
 * @MEMBERS: The member declarations for the mirrored structs.
 */
#define __struct_group(TAG, NAME, ATTRS, MEMBERS...) \
    union { \
        struct { MEMBERS } ATTRS; \
        struct TAG { MEMBERS } ATTRS NAME; \
    }

/**
 * struct_group - Wrap a set of declarations in a mirrored struct.
 * @name: The identifier name of the mirrored sub-struct.
 * @members: The member declarations for the mirrored structs.
 */
#define struct_group(NAME, MEMBERS...)	\
    __struct_group(/* no tag */, NAME, /* no attrs */, MEMBERS)

/**
 * struct_group_attr - Create a struct_group() with trailing attributes.
 * @NAME: The identifier name of the mirrored sub-struct.
 * @ATTRS: Any struct attributes to apply.
 * @MEMBERS: The member declarations for the mirrored structs.
 */
#define struct_group_attr(name, attrs, members...) \
    __struct_group(/* no tag */, NAME, ATTRS, MEMBERS)

#endif /* __ASSEMBLY__ */
#endif /* _STDDEF_H_ */
