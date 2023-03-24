/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_STRINGIFY_H_
#define _COMPILER_STRINGIFY_H_

/*
 * Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

#define ___stringify(x...)  #x
#define __stringify(x...)   ___stringify(x)

#endif	/* _COMPILER_STRINGIFY_H_ */
