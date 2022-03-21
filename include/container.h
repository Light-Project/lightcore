/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CONTAINER_H_
#define _CONTAINER_H_

/**
 * container_of - cast a member of a structure out to the containing structure.
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({              \
    const typeof(((type *)0)->member) *__mptr = (ptr);  \
    (type *)((char *)__mptr - offsetof(type,member));   \
})

#define container_of_safe(ptr, type, member) ({         \
    typeof(ptr) _ptr = (ptr);                           \
    _ptr ? container_of(_ptr, type, member) : NULL;     \
})

#endif  /* _CONTAINER_H_ */
