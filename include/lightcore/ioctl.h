/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_IOCTL_H_
#define _LIGHTCORE_IOCTL_H_

#ifndef IOCTL_SIZEBITS
# define IOCTL_SIZEBITS	    14
#endif
#ifndef IOCTL_DIRBITS
# define IOCTL_DIRBITS	    2
#endif
#ifndef IOCTL_FNBITS
# define IOCTL_FNBITS       8
#endif
#ifndef IOCTL_TYPEBITS
# define IOCTL_TYPEBITS     8
#endif

#ifndef IOCTL_DIR_NONE
# define IOCTL_DIR_NONE     0U
#endif
#ifndef IOCTL_DIR_OUTPUT
# define IOCTL_DIR_OUTPUT   1U
#endif
#ifndef IOCTL_DIR_INPUT
# define IOCTL_DIR_INPUT    2U
#endif

#define IOCTL_SIZE_SHIFT    (0)
#define IOCTL_DIR_SHIFT     (IOCTL_SIZE_SHIFT + IOCTL_SIZEBITS)
#define IOCTL_FN_SHIFT      (IOCTL_DIR_SHIFT + IOCTL_DIRBITS)
#define IOCTL_TYPE_SHIFT    (IOCTL_FN_SHIFT + IOCTL_FNBITS)

#define IOCTL(dir, type, fn, size) (    \
    ((size) << IOCTL_SIZE_SHIFT)    |   \
    ((dir) << IOCTL_DIR_SHIFT)      |   \
    ((fn) << IOCTL_FN_SHIFT)        |   \
    ((type) << IOCTL_TYPE_SHIFT)        \
)

#define IOCTL_NONE(type, fn)            IOCTL(IOCTL_DIR_NONE, type, fn, 0)
#define IOCTL_INPUT(type, fn, size)     IOCTL(IOCTL_DIR_INPUT, type, fn, size)
#define IOCTL_OUTPUT(type, fn, size)    IOCTL(IOCTL_DIR_OUTPUT, type, fn, size)
#define IOCTL_BIDIR(type, fn, size)     IOCTL(IOCTL_DIR_INPUT | IOCTL_DIR_OUTPUT, type, fn, size)

#endif /* _LIGHTCORE_IOCTL_H_ */
