/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_POINTER_H_
#define _COMPILER_POINTER_H_

#define _RET_IP_ (unsigned long)__builtin_return_address(0)
#define _THIS_IP_ ({ __label__ __here; __here: (unsigned long)&&__here; })

#endif  /* _COMPILER_POINTER_H_ */
